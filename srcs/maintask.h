/* 
 * File:   maintask.h
 * Author: georg
 *
 * Created on 28 maggio 2013, 19.31
 */

#ifndef MAINTASK_H
#define	MAINTASK_H

#include "kernel/task.h"


// this is the entry point for the user application and tasks
// there must be exactely one MainTask named parenttask
class MainTask : public Task<2048> {

 public:

     void OnRun();



};


#endif	/* MAINTASK_H */

