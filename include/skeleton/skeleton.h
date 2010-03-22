/**
   Copyright (c) 2010, Xiph.org Foundation

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __OGG_SKELETON_H__
#define __OGG_SKELETON_H__

/** @file skeleton.h
 * libskeleton public API
 */
 
#include <ogg/ogg.h>
#include <skeleton/skeleton_constants.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 
 */
typedef struct _OggSkeleton OggSkeleton;

#include <skeleton/skeleton_query.h>

/** 
 * 
 * @return 
 */
OggSkeleton* oggskel_new ();

/** 
 * 
 * @param skeleton 
 * @return
 */
OggSkeletonError oggskel_destroy (OggSkeleton* skeleton);

/** encoding */
int oggskel_encode_header (OggSkeleton* skeleton, ogg_packet *op);

//void oggskel_encode_ (OggSkeleton* skeleton);

/** decoding */
/**
 * 
 * @param skeleton skeleton handle
 * @parma op Ogg packet
 * @return
 */
int oggskel_decode_header (OggSkeleton* skeleton, const ogg_packet* op);

#ifdef __cplusplus
}
#endif

#endif /* __OGG_SKELETON_H__ */
