//
//  TerminalViewController.h
//  lsl-helloworld
//
//  Created by SCCN User on 4/13/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "lsl_c.h"

@interface TerminalViewController : UIViewController

@property (weak, nonatomic) IBOutlet UIButton *monitorButton;
@property (weak, nonatomic) IBOutlet UITextView *terminalText;
@property (weak, nonatomic) IBOutlet UITextView *infoText;
@property (weak, nonatomic) IBOutlet UILabel *terminalLabel;
@property (nonatomic)NSString* streamName;

@property lsl_streaminfo* info;

-(IBAction) monitor;
-(void)updateTerminalText:(NSString*) text;
-(void)listenThread;

@end
