//
//  TerminalViewController.m
//  lsl-helloworld
//
//  Created by SCCN User on 4/13/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

// This is an incomplete object for monitoring
// an lsl inlet. Only cft_types 1 and 4 (float and int64) are supported.
// This functionality seems of little
// use on a phone or tablet, but is included for
// completeness. It seems to be the case that
// iOS can't print to a text field object as fast as
// LSL data may stream in -- but this onject is a crude
// exploration of this aspect of the OS.

#import "TerminalViewController.h"
#define MAXINTDATA 2073600

@interface TerminalViewController ()

@property bool is_streaming_;

@end

@implementation TerminalViewController

-(void)updateTerminalText: (NSString*) text{
    
    [self.terminalText setText:text];
}

-(void) listenThread{
    
    NSLog(@"welcome to the listen thread...");
    lsl_inlet inlet;
    inlet = lsl_create_inlet(*self.info, 360, LSL_NO_PREFERENCE, 1);
    double ts;
    int errcode;
    
    int *intBuff = malloc(sizeof(int) * MAXINTDATA);
    float *floatBuff = malloc(sizeof(float) * lsl_get_channel_count(*self.info));

    while(self.is_streaming_ != false)
    {
        
        switch(lsl_get_channel_format(*self.info)){
            case cft_int64:
                ts = lsl_pull_sample_i(inlet, intBuff, MAXINTDATA, 0.0, &errcode);
                break;
            case cft_float32:
                ts = lsl_pull_sample_f(inlet, floatBuff, lsl_get_channel_count(*self.info), 0.0, &errcode);
                break;
            default:
                NSLog(@"Unsupported data type. Exiting listen Thread.");
                goto fail;
                break;
        }
        //stream info
        
        
        if(ts!=0.0)
            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                    [self.terminalText setText:[NSString stringWithFormat:@"Timestamp: %1.6f\n",ts]];
                        }];
        [NSThread sleepForTimeInterval:1];

        
    }
    
    goto fail;
    
fail:
    
    NSLog(@"Now leaving the listen thread");
    free(intBuff);
    free(floatBuff);
    lsl_destroy_inlet(inlet);

}

- (void)viewDidLoad {
    [super viewDidLoad];

    if(self.streamName)
    {
        //NSLog(@"did load the terminal view!");
    
        self.info = malloc(sizeof(lsl_streaminfo));
        lsl_resolve_byprop(self.info, 1, "name", [self.streamName UTF8String], 1,1.0);
        int fmt = lsl_get_channel_format(*self.info);
        //NSLog(@"hola!");

        
        // TODO: add more stream details here:
        [self.infoText setText:[NSString stringWithFormat:@"Streamn Info:\n\tName: %@\n\tType: %@\n\tChannel Count:%d \n\tChannel Format: %d", [NSString stringWithUTF8String:lsl_get_name(*self.info)], [NSString stringWithUTF8String:lsl_get_type(*self.info)], lsl_get_channel_count(*self.info), fmt]];
        self.is_streaming_ = false;
        
    }
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)setStreamName:(NSString *)streamName{
    
    _streamName = streamName;
    NSLog(@"!!!!!!!!!!!!!");
    NSLog(@"%@",_streamName);
    
}

-(IBAction)monitor{
    
    if(self.is_streaming_ == false)
    {
        self.is_streaming_ = true;
        [self.monitorButton setTitle:@"Stop Stream" forState:UIControlStateNormal];
        [NSThread detachNewThreadSelector:@selector(listenThread) toTarget:self withObject:nil];
        
    }
    else
    {
        self.is_streaming_ = false;
        [self.monitorButton setTitle:@"Start Stream" forState:UIControlStateNormal];

    }
    
    
}


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    self.is_streaming_ = false;
    if(self.info!=nil)free(self.info);
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}


@end
