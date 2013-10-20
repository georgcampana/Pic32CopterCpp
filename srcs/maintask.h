/*
 * Pic32CopterCpp -- A C++ microOS for the PIC32
 *
 * Copyright (C) 2012 - 2014, Georg Campana
 *
 * Georg Campana <g.campana(AT)stetel.com>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE.txt file
 * at the top of the source tree.
 *
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

