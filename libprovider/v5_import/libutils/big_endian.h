/*
 * Copyright 2013 - 2019 Xilinx, Inc.
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

#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H


#define _host_to_le32(n) bswap32(n)
#define _le32_to_host(n) bswap32(n)
#define _host_to_le16(n) bswap16(n)
#define _le16_to_host(n) bswap16(n)
#define _host_to_le32_inplace(data_p, count) bswap32_inplace(data_p, count)
#define _le32_to_host_inplace(data_p, count) bswap32_inplace(data_p, count)
#define _host_to_le16_inplace(data_p, count) bswap16_inplace(data_p, count)
#define _le16_to_host_inplace(data_p, count) bswap16_inplace(data_p, count)

#define _host_to_be32(n) (n)
#define _host_to_be16(n) (n)
#define _be32_to_host(n) (n)
#define _be16_to_host(n) (n)
#define _host_to_be32_inplace(data_p, count) (void)(0)
#define _host_to_be16_inplace(data_p, count) (void)(0)
#define _be32_to_host_inplace(data_p, count) (void)(0)
#define _be16_to_host_inplace(data_p, count) (void)(0)

#endif /* BIG_ENDIAN_H */
