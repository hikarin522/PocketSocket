/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "pwm.h"
#include "boost/preprocessor.hpp"

#define PWM(x, i, name) BOOST_PP_CAT(PWM_, BOOST_PP_CAT(i, BOOST_PP_CAT(_, name)))
#define PWM_WRITECOMP(x, i, t) PWM(x, i, WriteCompare1),PWM(x, i, WriteCompare2),
const WriteCompF PWM_WriteComp[24] = {BOOST_PP_REPEAT(12, PWM_WRITECOMP, null)};
#undef PWM_WRITECOMP
#undef PWM

pwm_state pwm_set(pwm_state s, int16 val) {
	if (val > 24 * 25) {
		val = 24 * 25;
	}
	
	return pwm_add(s, val - (s.comp + s.pwm * 25));
}

pwm_state pwm_add(pwm_state s, int16 count) {
	count += s.comp;
	for (;;) {
		if (count > 25) {
			s.comp = 25;
			PWM_WriteComp[s.pwm](s.comp);
			if (s.pwm >= 23) {
				return s;
			} else {
				++s.pwm;
				count -= 25;
			}
		} else if (count < 0) {
			s.comp = 0;
			PWM_WriteComp[s.pwm](s.comp);
			if (s.pwm == 0) {
				return s;
			} else {
				--s.pwm;
				count += 25;
			}
		} else {
			s.comp = count;
			PWM_WriteComp[s.pwm](s.comp);
			return s;
		}
	}
	return s;
}

pwm_state pwm_inc(pwm_state s) {
	if (s.comp >= 25 && s.pwm >= 23) {
		return s;
	} else if (s.comp >= 25 && s.pwm < 23) {
		++s.pwm;
		s.comp = 1;
		PWM_WriteComp[s.pwm](s.comp);
		return s;
	} else {
		++s.comp;
		PWM_WriteComp[s.pwm](s.comp);
		return s;
	}
}

pwm_state pwm_dec(pwm_state s) {
	if (s.comp == 0 && s.pwm == 0) {
		return s;
	} else if (s.comp == 0 && s.pwm > 0) {
		--s.pwm;
		s.comp = 24;
		PWM_WriteComp[s.pwm](s.comp);
		return s;
	} else {
		--s.comp;
		PWM_WriteComp[s.pwm](s.comp);
		return s;
	}
}

pwm_state createState(const uint8 pwm, const uint8 comp) {
	const pwm_state ret = {pwm, comp};
	return ret;
}

/* [] END OF FILE */
