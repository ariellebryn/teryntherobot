//
//  ViewController.m
//  whereareblünow
//
//  Created by Nate Parrott on 10/29/15.
//  Copyright © 2015 Nate Parrott. All rights reserved.
//

#import "ViewController.h"
#import "BluetoothCommunicator.h"

@interface ViewController ()

@property (nonatomic) BluetoothCommunicator *comm;
@property (nonatomic) IBOutlet UILabel *statusView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.comm = [BluetoothCommunicator new];
    __weak ViewController *weakSelf = self;
    self.comm.onStatusChanged = ^(NSString *status) {
        weakSelf.statusView.text = status;
    };
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
