//
//  AppDelegate.h
//  lsl-helloworld
//
//  Created by SCCN on 4/9/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCManager.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, strong) MCManager *mcManager;

@end

