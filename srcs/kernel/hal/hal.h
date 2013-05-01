/* 
 * File:   hal.h
 * Author: georg
 *
 * Created on 29 aprile 2013, 17.43
 */

#ifndef HAL_H
#define	HAL_H

#include <p32xxxx.h>
#include <plib.h>

class HAL {
public:
    static unsigned int DisableInterrupts();
    static void RestoreInterrupts(unsigned int oldstatus);
private:

};


inline unsigned int HAL::DisableInterrupts() { return 0;}
inline void HAL::RestoreInterrupts(unsigned int oldstatus) {}



#endif	/* HAL_H */

