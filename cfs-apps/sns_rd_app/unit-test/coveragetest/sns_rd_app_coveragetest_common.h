/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * Common definitions for all sns_rd_app coverage tests
 */

#ifndef SNS_RD_APP_COVERAGETEST_COMMON_H
#define SNS_RD_APP_COVERAGETEST_COMMON_H

/*
 * Includes
 */

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

#include "cfe.h"
#include "sns_rd_app_events.h"
#include "sns_rd_app.h"
#include "sns_rd_app_table.h"

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add((Test_##test), SensorRead_UT_Setup, SensorRead_UT_TearDown, #test)

/*
 * Setup function prior to every test
 */
void SensorRead_UT_Setup(void);

/*
 * Teardown function after every test
 */
void SensorRead_UT_TearDown(void);

#endif /* SNS_RD_APP_COVERAGETEST_COMMON_H */
