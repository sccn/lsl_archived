//
//  ViewController.h
//  lsl-helloworld
//
//  Created by David Medine on 2/13/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreMotion/CoreMotion.h>
#import "lsl_c.h"
#import "AppDelegate.h"

@interface ViewController : UIViewController  <MCBrowserViewControllerDelegate>

@property (nonatomic, strong) AppDelegate *appDelegate;

-(IBAction)showTime;
-(IBAction)showStreamCount;
-(IBAction)toggleAccelStream;

-(IBAction)browseForDevices:(id)sender;
-(IBAction)toggleVisibility:(id)sender;
-(IBAction)disconnect:(id)sender;


@property (weak, nonatomic) IBOutlet UIButton *toggleAccelButton;
@property (weak, nonatomic) IBOutlet UILabel *accelLabel;
@property (weak, nonatomic) IBOutlet UITextView *accelText;
@property (weak, nonatomic)IBOutlet UISwitch *swVisible;

@end



