//
//  BluetoothCommunicator.h
//  whereareblünow
//
//  Created by Nate Parrott on 10/29/15.
//  Copyright © 2015 Nate Parrott. All rights reserved.
//

#import <Foundation/Foundation.h>
@import CoreBluetooth;
@import UIKit;

@interface BluetoothCommunicator : NSObject

@property (nonatomic,copy) void (^onStatusChanged)(NSString *status);

@end
