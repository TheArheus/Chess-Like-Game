#if !defined(HMATH_H_)
#define HMATH_H_
union v2
{
    struct
    {
        r32 x, y;        
    };
    r32 E[2];
};

union v3
{
    struct
    {
        r32 x, y, z;        
    };
    struct
    {
        r32 r, g, b;        
    };
    struct
    {
        v2 xy;
        r32 _Ign0;
    };
    struct
    {
        r32 _Ign1;
        v2 yz;
    };
    r32 E[3];
};

union v4
{
    struct
    {
        r32 x, y, z, w;
    };
    struct
    {
        r32 r, g, b, a;
    };
    union
    {
        struct
        {
            v3 xyz;
        };
        r32 _Ign2;
    };
    union
    {
        struct
        {
            v3 rgb;
        };
        r32 _Ign3;
    };
    r32 E[4];
};

struct vN
{
    u32 N;
    r32* Values;

    r32& operator[](u32 Index)
    {
        return this->Values[Index];
    }
};

struct mNxM
{
    u32 N;
    u32 M;
    vN* Vectors;
};

inline r32
Lerp(r32 A, r32 t, r32 B)
{
    r32 Result = (1 - t)*A + t*B;
    return Result;
}

inline r32
Square(r32 A)
{
    return A*A;
}

inline r32
Clamp(r32 Min, r32 V, r32 Max)
{
    r32 Result = V;
    if(Result < Min)
    {
        Result = Min;
    }
    else if(Result > Max)
    {
        Result = Max;
    }

    return Result;
}

inline r32
Clamp01(r32 V)
{
    r32 Result = Clamp(0.0f, V, 1.0f);
    return Result;
}

inline r32
Step(r32 V, r32 Edge)
{
    r32 Result = (V < Edge) ? 0.0f : 1.0f;
    return Result;
}

inline r32
Smoothstep(r32 A, r32 x, r32 B)
{
    r32 Result = Clamp01((x - A) / (B - A));
    Result = x*x*(3 - 2 * x);
    return Result;
}

inline v2
Square(v2 A)
{
    v2 Result = {};

    Result.x = Square(A.x);
    Result.y = Square(A.y);

    return Result;
}

inline r32 
Inner(v2 A, v2 B)
{
    r32 Result = A.x*B.x + A.y*B.y;
    return Result;
}

inline r32
LengthSqr(v2 A)
{
    r32 Result = Inner(A, A);
    return Result;
}

inline r32
Length(v2 A)
{
    r32 Result = sqrtf(LengthSqr(A));
    return Result;
}

inline r32
Distance(v2 A, v2 B)
{
    r32 Result = sqrtf(Square(A.x - B.x) + Square(A.y - B.y));
    return Result;
}

inline r32
SafeRatioN(r32 Numerator, r32 Divisor, r32 N)
{
    r32 Result = N;
    if(Divisor != 0.0f)
    {
        Result = Numerator / Divisor;
    }
    return Result;
}

inline r32
SafeRatio0(r32 Num, r32 Div)
{
    r32 Result = SafeRatioN(Num, Div, 0.0f);
    return Result;
}

inline r32
SafeRatio1(r32 Num, r32 Div)
{
    r32 Result = SafeRatioN(Num, Div, 1.0f);
    return Result;
}

inline r32
SquareRoot(r32 V)
{
    r32 Result = (r32)sqrtf(V);
    return Result;
}

inline v2
V2(r32 X, r32 Y)
{
    v2 Result = {};

    Result.x = X;
    Result.y = Y;

    return Result;
}

inline v2
V2i(i32 X, i32 Y)
{
    v2 Result = {(r32)X, (r32)Y};

    return Result;
}

inline v3
V3(r32 X, r32 Y, r32 Z)
{
    v3 Result = {};

    Result.x = X;
    Result.y = Y;
    Result.z = Z;

    return Result;
}

inline v3
V3(r32 V)
{
    v3 Result = {};

    Result.x = V;
    Result.y = V;
    Result.z = V;

    return Result;
}

inline v3
V3(v2 XY, r32 Z)
{
    v3 Result = {};

    Result.xy = XY;
    Result.z = Z;

    return Result;
}

inline v3
V3(r32 X, v2 YZ)
{
    v3 Result = {};

    Result.x = X;
    Result.yz = YZ;

    return Result;
}

inline v4
V4(r32 V)
{
    v4 Result = {};

    Result.x = V;
    Result.y = V;
    Result.z = V;
    Result.w = V;

    return Result;
}

inline v4
V4(r32 X, r32 Y, r32 Z, r32 W)
{
    v4 Result = {};

    Result.x = X;
    Result.y = Y;
    Result.z = Z;
    Result.w = W;

    return Result;
}

inline v4
V4i(i32 X, i32 Y, i32 Z, i32 W)
{
    v4 Result = {};

    Result.x = (r32)X;
    Result.y = (r32)Y;
    Result.z = (r32)Z;
    Result.w = (r32)W;

    return Result;
}

inline vN
VN(r32 Size)
{
    vN Result = {};

    Result.N = Size;
    Result.Values = (r32*)calloc(sizeof(r32),Size);

    return Result;
}

inline vN
VN(v2 V)
{
    vN Result = {};

    Result.N = 2;
    Result.Values = (r32*)malloc(sizeof(r32)*Result.N);

    Result.Values[0] = V.x;
    Result.Values[1] = V.y;

    return Result;
}

inline vN
VN(v3 V)
{
    vN Result = {};

    Result.N = 3;
    Result.Values = (r32*)malloc(sizeof(r32)*Result.N);

    Result.Values[0] = V.x;
    Result.Values[1] = V.y;
    Result.Values[2] = V.z;

    return Result;
}

inline vN
VN(v4 V)
{
    vN Result = {};

    Result.N = 4;
    Result.Values = (r32*)malloc(sizeof(r32)*Result.N);

    Result.Values[0] = V.x;
    Result.Values[1] = V.y;
    Result.Values[2] = V.z;
    Result.Values[3] = V.w;

    return Result;
}

inline vN
VN(mNxM V)
{
    vN Result = {};

    if(V.M != 1 && V.Vectors[0].N > 1)
    {
        Result.N = V.M;
        Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

        for(u32 Index = 0;
            Index < Result.N;
            ++Index)
        {
            Result.Values[Index] = V.Vectors[Index].Values[0];
        }
    }
    else if (V.M > 1 && V.Vectors[0].N != 1)
    {
        Result.N = V.Vectors->N;
        Result.Values = (r32*)malloc(sizeof(r32)*Result.N);

        for(u32 Index = 0;
            Index < Result.N;
            ++Index)
        {
            Result.Values[Index] = V.Vectors->Values[Index];
        }
    }

    return Result;
}

inline vN
VN(vN V, u32 Size)
{
    vN Result = {};
    Result.N = Size;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < Result.N;
        ++Index)
    {
        if(Index < V.N)
        {
            Result.Values[Index] = V.Values[Index];
        }
        else
        {
            Result.Values[Index] = 0.0f;
        }
    }

    return Result;
}

inline mNxM
MNxM(u32 N, u32 M)
{
    mNxM Result = {};

    Result.N = N; // Rows
    Result.M = M; // Cols

    Result.Vectors = (vN*)calloc(sizeof(vN),Result.N);

    for(u32 VectorIndex = 0;
        VectorIndex < Result.N;
        ++VectorIndex)
    {
        Result.Vectors[VectorIndex] = VN(Result.M);
    }

    return Result;
}

inline mNxM
MNxM(vN V)
{
    mNxM Result = {};

    Result.N = V.N;
    Result.M = 1;
    Result.Vectors = &V;

    return Result;
}

inline v2 
operator+(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;

    return Result;
}

inline v2
operator+(v2 A, r32 B)
{
    v2 Result = {};

    Result.x = A.x + B;
    Result.y = A.y + B;

    return Result;
}

inline v2
operator+(r32 A, v2 B)
{
    B = B + A;

    return B;
}

inline v2&
operator+=(v2& A, v2 B)
{
    A = A + B;

    return A;
}

inline v2&
operator+=(v2& A, r32 B)
{
    A = A + B;

    return A;
}

inline v2 
operator-(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;

    return Result;
}

inline v2
operator-(v2 A, r32 B)
{
    v2 Result = {};

    Result.x = A.x - B;
    Result.y = A.y - B;

    return Result;
}

inline v2
operator-(r32 A, v2 B)
{
    B = B - A;

    return B;
}

inline v2
operator-(v2 A)
{
    v2 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;

    return Result;
}

inline v2&
operator-=(v2& A, v2 B)
{
    A = A - B;

    return A;
}

inline v2&
operator-=(v2& A, r32 B)
{
    A = A - B;

    return A;
}

inline v2
operator*(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = A.x*B.x;
    Result.y = A.y*B.y;

    return Result;
}

inline v2
operator*(v2 A, r32 B)
{
    v2 Result = {};

    Result.x = A.x*B;
    Result.y = A.y*B;

    return Result;
}

inline v2
operator*(r32 A, v2 B)
{
    B = B*A;

    return B;
}

inline v2&
operator*=(v2& A, v2 B)
{
    A = A*B;

    return A;
}

inline v2&
operator*=(v2& A, r32 B)
{
    A = A * B;

    return A;
}

inline v2
operator/(v2 A, v2 B)
{
    v2 Result = {};

    Result.x = SafeRatio0(A.x, B.x);
    Result.y = SafeRatio0(A.y, B.y);

    return Result;
}

inline v2
operator/(v2 A, r32 B)
{
    v2 Result = {};

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);

    return Result;
}

inline v2
operator/(r32 A, v2 B)
{
    B = B / A;
    return B;
}

inline v2 
Perp(v2 A)
{
    v2 Result = {A.y, -A.x};
    return Result;
}

inline v2 
PerpBack(v2 A)
{
    v2 Result = {-A.y, A.x};
    return Result;
}

inline v2
Normalize(v2 A)
{
    v2 Result = A * (1 / Length(A));
    return Result;
}

inline v2
NOZ(v2 A)
{
    v2 Result = V2(0, 0);
    r32 LengthSquared = LengthSqr(A);
    if(LengthSquared > Square(0.00001))
    {
        Result = A * (1 / SquareRoot(LengthSquared));
    }
    return Result;
}

inline v2
Normal(v2 A)
{
    v2 Result = {};
    Result = Normalize(Perp(A));
    return Result;
}

inline v2
rotate(v2 V, r32 A)
{
    v2 Result = {};

    Result.x = V.x*cosf(A) - V.y*sinf(A);
    Result.y = V.x*sinf(A) + V.y*cosf(A);

    return Result;
}

inline v2
rotate_c(v2 V, v2 C, r32 A)
{
    v2 Result = {};

    Result.x = C.x + (V.x - C.x)*cosf(A) - (V.y - C.y)*sinf(A);
    Result.y = C.y + (V.x - C.x)*sinf(A) + (V.y - C.y)*cosf(A);

    return Result;
}

inline r32
Cross(v2 A, v2 B)
{
    r32 Result = A.x*B.y - A.y*B.x;
    return Result;
}

inline v3 
operator+(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;

    return Result;
}

inline v3
operator+(v3 A, r32 B)
{
    v3 Result = {};

    Result.x = A.x + B;
    Result.y = A.y + B;
    Result.z = A.z + B;

    return Result;
}

inline v3
operator+(r32 A, v3 B)
{
    B = B + A;

    return B;
}

inline v3&
operator+=(v3& A, v3 B)
{
    A = A + B;

    return A;
}

inline v3&
operator+=(v3& A, r32 B)
{
    A = A + B;

    return A;
}

inline v3 
operator-(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;

    return Result;
}

inline v3
operator-(v3 A, r32 B)
{
    v3 Result = {};

    Result.x = A.x - B;
    Result.y = A.y - B;
    Result.z = A.z - B;

    return Result;
}

inline v3
operator-(r32 A, v3 B)
{
    B = B - A;

    return B;
}

inline v3
operator-(v3 A)
{
    v3 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;

    return Result;
}

inline v3&
operator-=(v3& A, v3 B)
{
    A = A - B;

    return A;
}

inline v3&
operator-=(v3& A, r32 B)
{
    A = A - B;

    return A;
}

inline v3
operator*(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = A.x*B.x;
    Result.y = A.y*B.y;
    Result.z = A.z*B.z;

    return Result;
}

inline v3
operator*(v3 A, r32 B)
{
    v3 Result = {};

    Result.x = A.x*B;
    Result.y = A.y*B;
    Result.z = A.z*B;

    return Result;
}

inline v3
operator*(r32 A, v3 B)
{
    B = B*A;

    return B;
}

inline v3&
operator*=(v3& A, v3 B)
{
    A = A*B;

    return A;
}

inline v3&
operator*=(v3& A, r32 B)
{
    A = A * B;

    return A;
}

inline v3
operator/(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = SafeRatio0(A.x, B.x);
    Result.y = SafeRatio0(A.y, B.y);
    Result.z = SafeRatio0(A.z, B.z);

    return Result;
}

inline v3
operator/(v3 A, r32 B)
{
    v3 Result = {};

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    Result.z = SafeRatio0(A.z, B);

    return Result;
}

inline v3
operator/(r32 A, v3 B)
{
    B = B / A;
    return B;
}

inline v3
rotate_x(v3 V, r32 A)
{
    v3 Result = {};

    Result.x = V.x;
    Result.y = V.y*cosf(A) - V.z*sinf(A);
    Result.z = V.y*sinf(A) + V.z*cosf(A);

    return Result;
}

inline v3
rotate_y(v3 V, r32 A)
{
    v3 Result = {};

    Result.x = V.x*cosf(A) - V.z*sinf(A);
    Result.y = V.y;
    Result.z = V.x*sinf(A) + V.z*cosf(A);

    return Result;
}

inline v3
rotate_z(v3 V, r32 A)
{
    v3 Result = {};

    Result.x = V.x*cosf(A) - V.y*sinf(A);
    Result.y = V.x*sinf(A) + V.y*sinf(A);
    Result.z = V.z;

    return Result;
}

inline v3
Cross(v3 A, v3 B)
{
    v3 Result = {};

    Result.x = (A.y*B.z - A.z*B.y);
    Result.y = (A.z*B.x - A.x*B.z);
    Result.z = (A.x*B.y - A.y*A.x);

    return Result;
}

inline v4 
operator+(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;

    return Result;
}

inline v4
operator+(v4 A, r32 B)
{
    v4 Result = {};

    Result.x = A.x + B;
    Result.y = A.y + B;
    Result.z = A.z + B;
    Result.w = A.w + B;

    return Result;
}

inline v4
operator+(r32 A, v4 B)
{
    B = B + A;

    return B;
}

inline v4&
operator+=(v4& A, v4 B)
{
    A = A + B;

    return A;
}

inline v4&
operator+=(v4& A, r32 B)
{
    A = A + B;

    return A;
}

inline v4 
operator-(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;

    return Result;
}

inline v4
operator-(v4 A, r32 B)
{
    v4 Result = {};

    Result.x = A.x - B;
    Result.y = A.y - B;
    Result.z = A.z - B;
    Result.w = A.w - B;

    return Result;
}

inline v4
operator-(r32 A, v4 B)
{
    B = B - A;

    return B;
}

inline v4
operator-(v4 A)
{
    v4 Result = {};

    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    Result.w = -A.w;

    return Result;
}

inline v4&
operator-=(v4& A, v4 B)
{
    A = A - B;

    return A;
}

inline v4&
operator-=(v4& A, r32 B)
{
    A = A - B;

    return A;
}

inline v4
operator*(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x*B.x;
    Result.y = A.y*B.y;
    Result.z = A.z*B.z;
    Result.w = A.w*B.w;

    return Result;
}

inline v4
operator*(v4 A, r32 B)
{
    v4 Result = {};

    Result.x = A.x*B;
    Result.y = A.y*B;
    Result.z = A.z*B;
    Result.w = A.w*B;

    return Result;
}

inline v4
operator*(r32 A, v4 B)
{
    B = B*A;

    return B;
}

inline v4&
operator*=(v4& A, v4 B)
{
    A = A*B;

    return A;
}

inline v4&
operator*=(v4& A, r32 B)
{
    A = A * B;

    return A;
}

inline v4
operator/(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = SafeRatio0(A.x, B.x);
    Result.y = SafeRatio0(A.y, B.y);
    Result.z = SafeRatio0(A.z, B.z);
    Result.w = SafeRatio0(A.w, B.w);

    return Result;
}

inline v4
operator/(v4 A, r32 B)
{
    v4 Result = {};

    Result.x = SafeRatio0(A.x, B);
    Result.y = SafeRatio0(A.y, B);
    Result.z = SafeRatio0(A.z, B);
    Result.w = SafeRatio0(A.w, B);

    return Result;
}

inline v4
operator/(r32 A, v4 B)
{
    B = B / A;
    return B;
}

inline v4 
Hadamard(v4 A, v4 B)
{
    v4 Result = {};

    Result.x = A.x*B.x;
    Result.y = A.y*B.y;
    Result.z = A.z*B.z;
    Result.w = A.w*B.w;

    return Result;
}

inline v4
Lerp(v4 A, r32 t, v4 B)
{
    v4 Result;

    Result.x = Lerp(A.x, t, B.x);
    Result.y = Lerp(A.y, t, B.y);
    Result.z = Lerp(A.z, t, B.z);
    Result.w = Lerp(A.w, t, B.w);

    return Result;
}

// Operators Vector size N

inline vN 
operator+(vN A, vN B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32)*Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] + B[Index];
    }

    return Result;
}

inline vN
operator+(vN A, r32 B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] + B;
    }

    return Result;
}

inline vN
operator+(r32 A, vN B)
{
    B = B + A;

    return B;
}

inline vN&
operator+=(vN& A, vN B)
{
    A = A + B;

    return A;
}

inline vN&
operator+=(vN& A, r32 B)
{
    A = A + B;

    return A;
}

inline vN 
operator-(vN A, vN B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] - B[Index];
    }

    return Result;
}

inline vN
operator-(vN A, r32 B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] - B;
    }

    return Result;
}

inline vN
operator-(r32 A, vN B)
{
    B = B - A;

    return B;
}

inline vN
operator-(vN A)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = -A[Index];
    }

    return Result;
}

inline vN&
operator-=(vN& A, vN B)
{
    A = A - B;

    return A;
}

inline vN&
operator-=(vN& A, r32 B)
{
    A = A - B;

    return A;
}

inline vN
operator*(vN A, vN B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] * B[Index];
    }

    return Result;
}

inline vN
operator*(vN A, r32 B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] * B;
    }

    return Result;
}

inline vN
operator*(r32 A, vN B)
{
    B = B*A;

    return B;
}

inline vN&
operator*=(vN& A, vN B)
{
    A = A*B;

    return A;
}

inline vN&
operator*=(vN& A, r32 B)
{
    A = A * B;

    return A;
}

inline vN
operator/(vN A, vN B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] / B[Index];
    }

    return Result;
}

inline vN
operator/(vN A, r32 B)
{
    vN Result = {};
    Result.N = A.N;
    Result.Values = (r32*)malloc(sizeof(r32) * Result.N);

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result[Index] = A[Index] / B;
    }

    return Result;
}

inline vN
operator/(r32 A, vN B)
{
    B = B / A;
    return B;
}

inline r32
Inner(vN A, vN B)
{
    r32 Result = 0.0f;

    for(u32 Index = 0;
        Index < A.N;
        ++Index)
    {
        Result += A[Index] * B[Index];
    }
    return Result;
}

inline r32
LengthSqr(vN A)
{
    r32 Result = Inner(A, A);
    return Result;
}

inline r32
Length(vN A)
{
    r32 Result = SquareRoot(LengthSqr(A));
    return Result;
}

inline mNxM
Transpose(mNxM M)
{
    mNxM Result = MNxM(M.M, M.N);
    for(u32 Rows = 0;
        Rows < M.N;
        ++Rows)
    {
        for(u32 Cols = 0;
            Cols < M.M;
            ++Cols)
        {
            Result.Vectors[Cols][Rows] = M.Vectors[Rows][Cols];
        }
    }

    return Result;
}

inline vN
SolveGaussSeidel(mNxM A, vN B)
{
    r32 N = B.N;
    vN Result = VN(N);
    
    for(u32 Iterations = 0;
        Iterations < N;
        ++Iterations)
    {
        for(u32 Index = 0;
            Index < N;
            ++Index)
        {
            if(A.Vectors[Index][Index] != 0.0f)
            {
                Result[Index] += (B[Index] / A.Vectors[Index][Index] - (Inner(A.Vectors[Index], Result) / A.Vectors[Index][Index]));
            }
        }
    }

    return Result;
}

inline vN
operator*(mNxM M, vN V)
{
    vN Result = V;
    if(V.N == M.M)
    {
        Result = VN(M.N);
        for(u32 Index = 0;
            Index < M.N;
            ++Index)
        {
            Result[Index] = Inner(V, M.Vectors[Index]);
        }
    }

    return Result;
};

inline mNxM
operator*(mNxM A, mNxM B)
{
    mNxM Result = MNxM(A.N, B.M);
    if (B.N == A.M)
    {
        for(u32 r = 0;
            r < A.N;
            ++r) // Rows of A
        {
            for(u32 c = 0;
                c < B.M;
                ++c) // Cols of B
            {
                for(u32 Index = 0;
                    Index < A.M;
                    ++Index) // Cols of A, Rows of B
                {
                    Result.Vectors[r][c] += A.Vectors[r][Index] * B.Vectors[Index][c];
                }
            }
        }
    }

    return Result;
}

internal v4
UnpackRGBA(u32 Color)
{
    v4 Result = V4((Color >>  0) & 0xFF,
                   (Color >>  8) & 0xFF,
                   (Color >> 16) & 0xFF,
                   (Color >> 24) & 0xFF);
    return Result;
}

internal v4
UnpackBGRA(u32 Color)
{
    v4 Result = V4((Color >> 16) & 0xFF,
                   (Color >>  8) & 0xFF,
                   (Color >>  0) & 0xFF,
                   (Color >> 24) & 0xFF);
    return Result;
}

// NOTE: In this project I need only this one
internal u32 
PackBGRA(v4 Color)
{
    u32 Result = ((((u32)roundf(Color.a)) << 24) | 
                  (((u32)roundf(Color.r)) << 16) |
                  (((u32)roundf(Color.g)) <<  8) |
                  (((u32)roundf(Color.b)) <<  0));
    return Result;
}

internal u32 
PackRGBA(v4 Color)
{
    u32 Result = ((((u32)roundf(Color.a)) << 24) | 
                  (((u32)roundf(Color.b)) << 16) |
                  (((u32)roundf(Color.g)) <<  8) |
                  (((u32)roundf(Color.r)) <<  0));
    return Result;
}

internal v4 
SRGBTo1Linear(v4 A)
{
    v4 Result = {};

    Result.r = Square(A.r / 255.0f);
    Result.g = Square(A.g / 255.0f);
    Result.b = Square(A.b / 255.0f);
    Result.a = A.a / 255.0f;

    return Result;
}

internal v4 
LinearTo255SRGB(v4 A)
{
    v4 Result = {};

    Result.r = sqrtf(A.r) * 255.0f;
    Result.g = sqrtf(A.g) * 255.0f;
    Result.b = sqrtf(A.b) * 255.0f;
    Result.a = A.a * 255.0f;

    return Result;
}

struct rectangle2
{
    v2 Min;
    v2 Max;
};

internal v2
GetDim(rectangle2 A)
{
    v2 Result = A.Max - A.Min;

    return Result;
}

internal v2
GetCenter(rectangle2 A)
{
    v2 Result = 0.5f*(A.Max + A.Min);

    return Result;
}

internal v2
GetHalfDim(rectangle2 A)
{
    v2 Result = 0.5f * GetDim(A);

    return Result;
}

internal rectangle2
RectangleMinMax(v2 Min, v2 Max)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Max;

    return Result;
}

internal rectangle2
RectangleMinDim(v2 Min, v2 Dims)
{
    rectangle2 Result;

    Result.Min = Min;
    Result.Max = Min + Dims;

    return Result;
}

internal rectangle2
RectangleCenterHalfDim(v2 Center, v2 HalfDim)
{
    rectangle2 Result;

    Result.Min = Center - HalfDim;
    Result.Max = Center + HalfDim;

    return Result;
}

internal rectangle2
RectangleAddRadius(rectangle2 A, v2 Radius)
{
    rectangle2 Result;

    Result.Min = A.Min - Radius;
    Result.Max = A.Max + Radius;

    return Result;
}

internal rectangle2
RectangleAddMin(rectangle2 A, v2 V)
{
    rectangle2 Result;

    Result.Min = A.Min + V;
    Result.Max = A.Max;

    return Result;
}

internal rectangle2
RectangleAddMax(rectangle2 A, v2 V)
{
    rectangle2 Result;

    Result.Min = A.Min;
    Result.Max = A.Max + V;

    return Result;
}

internal rectangle2
RectangleDisplace(rectangle2 A, v2 P)
{
    rectangle2 Result;

    Result.Min += P;
    Result.Max += P;

    return Result;
}

internal rectangle2
RectangleCenterDim(v2 Center, v2 Dims)
{
    rectangle2 Result = RectangleCenterHalfDim(Center, 0.5f*Dims);
    return Result;
}

internal b32
IsInRectangle(rectangle2 A, v2 Point)
{
    b32 Result = ((Point.x >= A.Min.x) && 
                  (Point.y >= A.Min.y) &&
                  (Point.x <  A.Max.x) &&
                  (Point.y <  A.Max.y));
    return Result;
}

#endif
