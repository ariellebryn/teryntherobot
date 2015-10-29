//
//  BluetoothCommunicator.m
//  whereareblünow
//
//  Created by Nate Parrott on 10/29/15.
//  Copyright © 2015 Nate Parrott. All rights reserved.
//

#import "BluetoothCommunicator.h"

@interface BluetoothCommunicator () <CBCentralManagerDelegate, CBPeripheralDelegate>

@property (nonatomic) CBCentralManager *mgr;
@property (nonatomic) CBPeripheral *peripheral;
@property (nonatomic) CBService *service;

@end

@implementation BluetoothCommunicator

- (instancetype)init {
    self = [super init];
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
        [self.mgr cancelPeripheralConnection:_peripheral];
    }
    _peripheral = peripheral;
    [self.mgr connectPeripheral:peripheral options:nil];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    NSLog(@"Connected");
    for (CBService *service in peripheral.services) {
        NSLog(@"%@", service.UUID.UUIDString);
    }
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"Failed to connect");
}

@end
