/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "formatting.h"

#include <float.h>
#include <time.h>

int ftostr_l(char* restrict str, size_t size, float f, locale_t locale) {
#ifdef HAVE_SNPRINTF_L
	return snprintf_l(str, size, locale, "%.*g", FLT_DIG, f);
#elif defined(HAVE_LOCALE)
	locale_t old = uselocale(locale);
	int res = snprintf(str, size, "%.*g", FLT_DIG, f);
	uselocale(old);
	return res;
#elif defined(HAVE_SETLOCALE)
	char* old = setlocale(LC_NUMERIC, locale);
	int res = snprintf(str, size, "%.*g", FLT_DIG, f);
	setlocale(LC_NUMERIC, old);
	return res;
#else
	UNUSED(locale);
	return snprintf(str, size, "%.*g", FLT_DIG, f);
#endif
}

#ifndef HAVE_STRTOF_L
float strtof_l(const char* restrict str, char** restrict end, locale_t locale) {
#ifdef HAVE_LOCALE
	locale_t old = uselocale(locale);
	float res = strtof(str, end);
	uselocale(old);
	return res;
#elif defined(HAVE_SETLOCALE)
	char* old = setlocale(LC_NUMERIC, locale);
	float res = strtof(str, end);
	setlocale(LC_NUMERIC, old);
	return res;
#else
	UNUSED(locale);
	return strtof(str, end);
#endif
}
#endif

int ftostr_u(char* restrict str, size_t size, float f) {
#if HAVE_LOCALE
	locale_t l = newlocale(LC_NUMERIC_MASK, "C", 0);
#else
	locale_t l = "C";
#endif
	int res = ftostr_l(str, size, f, l);
#if HAVE_LOCALE
	freelocale(l);
#endif
	return res;
}

float strtof_u(const char* restrict str, char** restrict end) {
#if HAVE_LOCALE
	locale_t l = newlocale(LC_NUMERIC_MASK, "C", 0);
#else
	locale_t l = "C";
#endif
	float res = strtof_l(str, end, l);
#if HAVE_LOCALE
	freelocale(l);
#endif
	return res;
}

#ifndef HAVE_LOCALTIME_R
#ifdef PSP2
#include <psp2/rtc.h>
#endif

struct tm* localtime_r(const time_t* t, struct tm* date) {
#ifdef _WIN32
	localtime_s(date, t);
	return date;
#elif defined(PSP2)
	SceRtcTime sceRtc;
	sceRtcSetTime_t(&sceRtc, *t);
	date->tm_year = sceRtc.year;
	date->tm_mon = sceRtc.month;
	date->tm_mday = sceRtc.day;
	date->tm_hour = sceRtc.hour;
	date->tm_min = sceRtc.minutes;
	date->tm_sec = sceRtc.seconds;
	date->tm_wday = sceRtcGetDayOfWeek(sceRtc.year, sceRtc.month, sceRtc.day);
	return date;
#else
#warning localtime_r not emulated on this platform
	return 0;
#endif
}
#endif
