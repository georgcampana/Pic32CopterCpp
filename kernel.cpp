/* 
 * File:   kernel.cpp
 * Author: gcamp_000
 * 
 * Created on 28 aprile 2013, 17.32
 */

#include "include/kernel.h"

Kernel::Kernel() {



}

TaskBase* Kernel::runningnow = 0;

List Kernel::readytasks;
List Kernel::waitingtasks;