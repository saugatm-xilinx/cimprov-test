/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* sfutils version numbers */

#ifndef __SFUTILS_VERSION_H__
#define __SFUTILS_VERSION_H__

/* These numbers are updated manually, each time the software is "released" */

#define SFU_MAJ_RELEASE_VERSION            (8)
#define SFU_MIN_RELEASE_VERSION            (1)
#define SFU_MAINT_RELEASE_VERSION          (1)
#define SFU_INCREMENTING_VERSION           (1001)
#define SFU_VERSION_STRING                 "v8.1.1.1001"

/* Don't show incrementing version (build number) by default in Windows .rc files */
#define SFU_RC_VERSION_STRING              "v8.1.1"

#define SFU_COPYRIGHT_STRING \
  "Copyright Solarflare Communications 2006-2018, Level 5 Networks 2002-2005 "
#define SFU_COMPANY_NAME                   "Solarflare"
#define SFU_PRODUCT_NAME                   "Solarflare"

#endif  /* __SFUTILS_VERSION_H__ */
