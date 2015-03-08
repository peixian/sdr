/*
 * Real horizontal addition
 */
inline __m128 sse_hadd_R(__m128 in){
    __m128 accum;
    accum = _mm_hadd_ps(in, in);
    return _mm_hadd_ps(accum, accum);
}

inline __m128 avx_hadd_R(__m256 in){
    __m128 res1 = _mm256_extractf128_ps(in, 0);
    __m128 res2 = _mm256_extractf128_ps(in, 1);

    res1 = _mm_hadd_ps(res1, res1);
    res1 = _mm_hadd_ps(res1, res1);

    res2 = _mm_hadd_ps(res2, res2);
    res2 = _mm_hadd_ps(res2, res2);

    return _mm_add_ss(res1, res2);
}

/*
 * Real dot products
 */
inline __m128 sse_dotprod_R(int num, float *a, float *b){
    int i;
    __m128 accum = _mm_setzero_ps();

    for(i=0; i<num; i+=4){
        //Load the needed vectors
        __m128 coeff = _mm_loadu_ps(a + i);
        __m128 val   = _mm_loadu_ps(b + i);

        //Multiply and acumulate
        accum = _mm_add_ps(accum, _mm_mul_ps(coeff, val));
    }
    return accum;
}

inline __m256 avx_dotprod_R(int num, float *a, float *b){
    int i;
    __m256 accum = _mm256_setzero_ps();

    for(i=0; i<num; i+=8){

        //Load the needed vectors
        __m256 coeff = _mm256_loadu_ps(a + i);
        __m256 val   = _mm256_loadu_ps(b + i);

        //Multiply and acumulate
        accum = _mm256_add_ps(accum, _mm256_mul_ps(coeff, val));
    }
    return accum;
}

/*
 * Complex horizontal addition
 */
inline __m128 sse_hadd_C(__m128 in) {
    __m128 accum = _mm_shuffle_ps(in, in, 0b11011000);
    return _mm_hadd_ps(accum, accum);
}

inline __m128 avx_hadd_C(__m256 in) {
    __m256 accum    = _mm256_permute_ps(in, _MM_SHUFFLE(3, 1, 2, 0));
    __m128 accum_hi = _mm256_extractf128_ps(accum, 1);
    __m128 accum_lo = _mm256_extractf128_ps(accum, 0);
    __m128 added    = _mm_hadd_ps(accum_lo, accum_hi);
    added           = _mm_permute_ps(added, _MM_SHUFFLE(3, 1, 2, 0));
    added           = _mm_hadd_ps(added, added);
    return added;
}

/*
 * Complex dot products
 */

inline __m128 sse_dotprod_C(int num, float *coeffs, float *startPtr){
    int i;
    __m128 accum1 = _mm_setzero_ps();
    __m128 accum2 = _mm_setzero_ps();

    for(i=0; i<num; i+=4){

        //Load the needed vectors
        __m128 coeff  = _mm_loadu_ps(coeffs + i);
        __m128 coeff1 = _mm_shuffle_ps(coeff, coeff, 0x50);
        __m128 coeff2 = _mm_shuffle_ps(coeff, coeff, 0xfa);
        __m128 val1   = _mm_loadu_ps(startPtr + 2 * i);
        __m128 val2   = _mm_loadu_ps(startPtr + 2 * i + 4);

        //Multiply and acumulate
        accum1 = _mm_add_ps(accum1, _mm_mul_ps(coeff1, val1));
        accum2 = _mm_add_ps(accum2, _mm_mul_ps(coeff2, val2));
    }
    __m128 accum = _mm_add_ps(accum1, accum2);
    return accum;
}

/*
 * Real symmetric dot products
 */

inline __m128 sse_sym_dotprod_R(int num, float *a, float *b){
    int i;
    __m128 accum = _mm_setzero_ps();

    float *startPtr = b;
    float *endPtr = b + num * 2 - 4;
    for(i=0; i<num; i+=4){

        //Load the needed vectors
        __m128 coeff = _mm_loadu_ps(a + i);
        __m128 val1  = _mm_loadu_ps(startPtr + i);
        __m128 val2  = _mm_loadu_ps(endPtr   - i);
        val2         = _mm_permute_ps(val2, _MM_SHUFFLE(0, 1, 2, 3));

        //Multiply and acumulate
        accum = _mm_add_ps(accum, _mm_mul_ps(coeff, _mm_add_ps(val1, val2)));
    }

    return accum;
}

inline __m256 avx_sym_dotprod_R(int num, float *a, float *b){
    int i;
    __m256 accum = _mm256_setzero_ps();

    float *startPtr = b;
    float *endPtr   = b + num * 2 - 8;
    for(i=0; i<num; i+=8){

        //Load the needed vectors
        __m256 coeff = _mm256_loadu_ps(a + i);
        __m256 val1  = _mm256_loadu_ps(startPtr + i);
        __m256 val2  = _mm256_loadu_ps(endPtr   - i);
        val2         = _mm256_permute2f128_ps(val2, val2, 0x01);
        val2         = _mm256_permute_ps(val2, _MM_SHUFFLE(0, 1, 2, 3));

        //Multiply and acumulate
        accum = _mm256_add_ps(accum, _mm256_mul_ps(coeff, _mm256_add_ps(val1, val2)));
    }

    return accum;
}
