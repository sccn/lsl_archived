//
//  InletsTableViewController.h
//  lsl-helloworld
//
//  Created by SCCN User on 4/10/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "lsl_c.h"
#define MAX_STREAMS 64

@interface InletsTableViewController : UITableViewController <UITableViewDataSource, UITableViewDelegate>

@property lsl_streaminfo *lsl_streams;
@property int num_streams;
@property lsl_streaminfo streamInfo;


@end
