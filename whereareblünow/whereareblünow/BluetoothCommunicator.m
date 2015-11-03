//
//  BluetoothCommunicator.m
//  whereareblünow
//
//  Created by Nate Parrott on 10/29/15.
//  Copyright © 2015 Nate Parrott. All rights reserved.
//

#import "BluetoothCommunicator.h"
#import <Firebase.h>

@interface BluetoothCommunicator () <CBCentralManagerDelegate, CBPeripheralDelegate>

@property (nonatomic) CBCentralManager *mgr;
@property (nonatomic) CBPeripheral *peripheral;
@property (nonatomic) CBService *service;
@property (nonatomic) CBCharacteristic *sendChar, *receiveChar, *disconnectChar;
@property (nonatomic) Firebase *firebase;
@property (nonatomic) NSInteger currentDirection;

@end

@implementation BluetoothCommunicator

- (instancetype)init {
    self = [super init];
    self.onStatusChanged = ^(NSString *s) {
        NSLog(@"Status: %@", s);
    };
    self.firebase = [[Firebase alloc] initWithUrl:@"https://teryntherobot.firebaseio.com"];
    __weak BluetoothCommunicator *weakSelf = self;
    [[self.firebase childByAppendingPath:@"direction"] observeEventType:FEventTypeValue withBlock:^(FDataSnapshot *snapshot) {
        id val = [snapshot value];
        if ([val isKindOfClass:[NSNumber class]]) {
            weakSelf.currentDirection = [val integerValue];
        }
    }];
    self.mgr = [[CBCentralManager alloc] initWithDelegate:self queue:dispatch_get_main_queue()];
    return self;
}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
    NSLog(@"Updated state: %li", central.state);
    if (central.state == CBCentralManagerStatePoweredOn) {
        [self.mgr scanForPeripheralsWithServices:nil options:nil];
    }
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
    if ([advertisementData[CBAdvertisementDataLocalNameKey] isEqualToString:@"ourduino"]) {
        self.peripheral = peripheral;
    }
}

- (void)setPeripheral:(CBPeripheral *)peripheral {
    if (_peripheral) {
        _peripheral.delegate = nil;
        [self.mgr cancelPeripheralConnection:_peripheral];
    }
    _peripheral = peripheral;
    self.onStatusChanged(peripheral ? @"Connecting" : @"Disconnected");
    peripheral.delegate = self;
    [self.mgr connectPeripheral:peripheral options:nil];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    NSLog(@"Connected");
    [peripheral discoverServices:nil];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"Failed to connect");
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    self.peripheral = nil;
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
    for (CBService *service in peripheral.services) {
        NSLog(@"Found service: %@", service.UUID.UUIDString);
        [peripheral discoverCharacteristics:nil forService:service];
    }
}

- (void)sendNumber:(NSInteger)num {
    char d[1];
    d[0] = (char)num;
    NSData *data = [NSData dataWithBytes:&d length:1];
    [self.peripheral writeValue:data forCharacteristic:self.sendChar type:CBCharacteristicWriteWithoutResponse];
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
    for (CBCharacteristic *chr in service.characteristics) {
        NSString *str = chr.UUID.UUIDString;
        if ([str isEqualToString:@"2221"]) {
            self.receiveChar = chr;
        } else if ([str isEqualToString:@"2222"]) {
            self.sendChar = chr;
            NSLog(@"Got send char");
            self.onStatusChanged(@"Ready");
            [self sendNumber:self.currentDirection];
        } else if ([str isEqualToString:@"2223"]) {
            self.disconnectChar = chr;
        }
    }
}

- (void)setCurrentDirection:(NSInteger)currentDirection {
    _currentDirection = currentDirection;
    if (self.sendChar) {
        [self sendNumber:currentDirection];
    }
}

@end
