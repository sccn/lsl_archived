//
//  ViewController.m
//  lsl-helloworld
//
//  Created by David Medine on 2/13/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import "ViewController.h"


//#include "<boost/functional/hash.hpp>

@interface ViewController ()

@property bool accelIsStreaming_;
-(void)streamAccelerometerData;

@property double *xyz; // array of acceleration values
@property double ts; // current timestamp

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.xyz = malloc(sizeof(double) *3);
    self.accelIsStreaming_ = false;
    
    _appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [[_appDelegate mcManager] setupPeerAndSessionWithDisplayName:[UIDevice currentDevice].name];
    [[_appDelegate mcManager] advertisSelf:_swVisible.isOn];
    
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)showTime{
    
    double time = lsl_local_clock();
    //double time = 1.2;
    NSString *time_str = [NSString stringWithFormat:@"The time is now %1.6f", time];
    
    UIAlertView *helloWorldAlert = [[UIAlertView alloc]
                                    initWithTitle:@"Hello LSL"  message:time_str delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [helloWorldAlert show];
}

-(IBAction)showStreamCount{
    
    lsl_streaminfo *streams;
    int max_streams = 64;
    int num_streams;
    streams = malloc(sizeof(lsl_streaminfo) * max_streams);
    
    num_streams = lsl_resolve_all(streams, max_streams, 1.0);
    
    UIAlertView *numStreamsAlert = [[UIAlertView alloc]
                                    initWithTitle:@"Hello LSL" message:[NSString stringWithFormat:@"There are %d streams", num_streams] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    
    
    [numStreamsAlert show];
    
}

-(IBAction)browseForDevices:(id)sender{
    
    [[_appDelegate mcManager] setupMCBrowser];
    [[[_appDelegate mcManager] browser] setDelegate:self];
    [self presentViewController:[[_appDelegate mcManager] browser] animated:YES completion:nil];
    
}

static const NSTimeInterval samplePer = .1; // sampling period

-(void)streamAccelerometerData{
    
    lsl_outlet outlet;
    lsl_streaminfo info;
    
    double SR = (double)(1.0/samplePer); // sample rate

    
    // setup the outlet
    info = lsl_create_streaminfo("iOS Accelerometer", "EEG", 3, SR, cft_double64, "someiPhoneorwhatever");
    outlet = lsl_create_outlet(info, 0, 360);


    
    // set up the accelerometer object
    CMMotionManager *mManager = [[CMMotionManager alloc] init];
    mManager.accelerometerUpdateInterval = samplePer;
    [mManager startAccelerometerUpdates]; // I assume this blocks correctly for me (ie every sample period)
    // the apple documentation is not clear on this point.
    
    // now that the accelerometer data is being updated in the background (?)
    // spit out lsl messages
    while(self.accelIsStreaming_==true)
    {
        // pack the data into an array and send it out
//        self.xyz[0] = mManager.accelerometerData.acceleration.x;
//        self.xyz[1] = mManager.accelerometerData.acceleration.y;
//        self.xyz[2] = mManager.accelerometerData.acceleration.z;
//        self.ts = lsl_local_clock();
//        lsl_push_sample_d(outlet, self.xyz);
        
//        // print the data to the screen
        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
            [self.accelText setText:[NSString stringWithFormat:@"X: %1.6f\nY: %1.6f\nZ: %1.6f\nTime: %1.6f", mManager.accelerometerData.acceleration.x, mManager.accelerometerData.acceleration.x, mManager.accelerometerData.acceleration.x, mManager.accelerometerData.acceleration.x]];//self.xyz[0], self.xyz[1], self.xyz[2], self.ts]];
        }];
        
    }

    [mManager stopAccelerometerUpdates];
    lsl_destroy_outlet(outlet);
}

-(IBAction)toggleAccelStream{
    
    if(self.accelIsStreaming_ == false)
    {
        // Request to turn on accelerometer and begin receiving accelerometer events

        
        [self.accelLabel setEnabled:true];
        [self.toggleAccelButton setTitle:@"Stop Accelerometer Stream" forState:UIControlStateNormal];
        self.accelIsStreaming_ = true;
        [NSThread detachNewThreadSelector:@selector(streamAccelerometerData) toTarget:self withObject:nil];
    }
    else{
        self.accelIsStreaming_ = false;
        [self.accelLabel setEnabled:false];
        [self.toggleAccelButton setTitle:@"Start Accelerometer Stream" forState:UIControlStateNormal];

    }
}

-(void) viewDidDisappear{
    
    free(self.xyz);
}


@end

