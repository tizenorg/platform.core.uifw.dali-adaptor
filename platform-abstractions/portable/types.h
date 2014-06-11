#ifndef _DALI_INTERNAL_PLATFORM_TYPES_H_
#define _DALI_INTERNAL_PLATFORM_TYPES_H_
/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES

// EXTERNAL INCLUDES
#include <utility>

namespace Dali
{
namespace Internal
{
namespace Platform
{

/**
 * @brief Pass a couple of unsigned integers around and access components as (x, y).
 */
typedef std::pair<unsigned, unsigned> Tuple2u;
// Getters:
unsigned x( const Tuple2u& tupple2 ) { return tupple2.first; }
unsigned y( const Tuple2u& tupple2 ) { return tupple2.second; }
// Setters:
unsigned& x( Tuple2u& tupple2 ) { return tupple2.first; }
unsigned& y( Tuple2u& tupple2 ) { return tupple2.second; }

}
}
}


#endif /* _DALI_INTERNAL_PLATFORM_TYPES_H_ */
