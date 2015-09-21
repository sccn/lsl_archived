//
//  InletsTableViewController.m
//  lsl-helloworld
//
//  Created by SCCN User on 4/10/15.
//  Copyright (c) 2015 SCCN. All rights reserved.
//

#import "InletsTableViewController.h"
#import "TerminalViewController.h"

@interface InletsTableViewController ()

@property NSMutableArray* stream_names;
@property NSString* streamName;

@end

@implementation InletsTableViewController

- (void)viewDidLoad {
    [super viewDidLoad];
   
    int i;
    // get streams
    self.lsl_streams = malloc(sizeof(lsl_streaminfo) * MAX_STREAMS);
    self.num_streams = lsl_resolve_all(self.lsl_streams, MAX_STREAMS, 1.0);

    // test with the fake data
//    self.num_streams = 5;
//    const char *fake = "abcde";
    
    // initialize the mutable array
    self.stream_names = [[NSMutableArray alloc] initWithObjects:nil];
      // fill the name array with stream names
    for(i=0; i<self.num_streams; i++)
    {
        // real:
        // OMG!!                       convert the stream name to NSString     will this work????
        [self.stream_names addObject: [NSString stringWithUTF8String:lsl_get_name(self.lsl_streams[i])]];
               // for testting:
//        [self.stream_names addObject: [NSString stringWithUTF8String:fake+i]];
        NSLog(@"%@", self.stream_names);
    }
    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
    
    free(self.lsl_streams);
    self.lsl_streams = NULL;
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1; // only need 1 section
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    //NSLog(@"There are %lu streams", (unsigned long)[self.stream_names count]);
    return [self.stream_names count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    
    static NSString *cellID = @"Cell ID";
    
    [tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:cellID];
    
    NSString *cell_name = [self.stream_names objectAtIndex:[indexPath row]];
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: cellID forIndexPath:indexPath];
    
    [cell.textLabel setText:cell_name];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    // what will this do, I wonder?
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    self.streamName = [self.stream_names objectAtIndex:[indexPath row]];
    NSLog(@"%@", self.streamName);
    [self performSegueWithIdentifier:@"ToTerminal" sender:self];
    
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/


#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    
    if([segue.destinationViewController isKindOfClass:[TerminalViewController class]])
    {

        TerminalViewController *tvc = [segue destinationViewController];
        [tvc setStreamName:self.streamName];
        [self setStreamName:nil];
    }
    
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}


@end
