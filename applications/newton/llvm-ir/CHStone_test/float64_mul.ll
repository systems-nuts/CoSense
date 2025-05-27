; ModuleID = 'float64_mul.ll'
source_filename = "dfmul/float64_mul.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@float_rounding_mode = dso_local global i32 0, align 4, !dbg !0
@float_exception_flags = dso_local global i32 0, align 4, !dbg !14
@_ZZL19countLeadingZeros32jE21countLeadingZerosHigh = internal constant <{ [128 x i32], [128 x i32] }> <{ [128 x i32] [i32 8, i32 7, i32 6, i32 6, i32 5, i32 5, i32 5, i32 5, i32 4, i32 4, i32 4, i32 4, i32 4, i32 4, i32 4, i32 4, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 3, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 2, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1, i32 1], [128 x i32] zeroinitializer }>, align 16, !dbg !18

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z19shift64RightJammingyiPy(i64 %0, i32 %1, i64* %2) #0 !dbg !278 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !283, metadata !DIExpression()), !dbg !284
  call void @llvm.dbg.value(metadata i32 %1, metadata !285, metadata !DIExpression()), !dbg !284
  call void @llvm.dbg.value(metadata i64* %2, metadata !286, metadata !DIExpression()), !dbg !284
  %4 = icmp eq i32 %1, 0, !dbg !287
  br i1 %4, label %5, label %6, !dbg !289

5:                                                ; preds = %3
  call void @llvm.dbg.value(metadata i64 %0, metadata !290, metadata !DIExpression()), !dbg !284
  br label %22, !dbg !291

6:                                                ; preds = %3
  %7 = icmp slt i32 %1, 64, !dbg !293
  br i1 %7, label %8, label %18, !dbg !295

8:                                                ; preds = %6
  %9 = zext i32 %1 to i64, !dbg !296
  %10 = lshr i64 %0, %9, !dbg !296
  %11 = sub nsw i32 0, %1, !dbg !298
  %12 = and i32 %11, 63, !dbg !299
  %13 = zext i32 %12 to i64, !dbg !300
  %14 = shl i64 %0, %13, !dbg !300
  %15 = icmp ne i64 %14, 0, !dbg !301
  %16 = zext i1 %15 to i64, !dbg !302
  %17 = or i64 %10, %16, !dbg !303
  call void @llvm.dbg.value(metadata i64 %17, metadata !290, metadata !DIExpression()), !dbg !284
  br label %21, !dbg !304

18:                                               ; preds = %6
  %19 = icmp ne i64 %0, 0, !dbg !305
  %20 = zext i1 %19 to i64, !dbg !307
  call void @llvm.dbg.value(metadata i64 %20, metadata !290, metadata !DIExpression()), !dbg !284
  br label %21

21:                                               ; preds = %18, %8
  %.0 = phi i64 [ %17, %8 ], [ %20, %18 ], !dbg !308
  call void @llvm.dbg.value(metadata i64 %.0, metadata !290, metadata !DIExpression()), !dbg !284
  br label %22

22:                                               ; preds = %21, %5
  %.1 = phi i64 [ %0, %5 ], [ %.0, %21 ], !dbg !309
  call void @llvm.dbg.value(metadata i64 %.1, metadata !290, metadata !DIExpression()), !dbg !284
  store i64 %.1, i64* %2, align 8, !dbg !310
  ret void, !dbg !311
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10mul64To128yyPyS_(i64 %0, i64 %1, i64* %2, i64* %3) #0 !dbg !312 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !315, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64 %1, metadata !317, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64* %2, metadata !318, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64* %3, metadata !319, metadata !DIExpression()), !dbg !316
  %5 = trunc i64 %0 to i32, !dbg !320
  call void @llvm.dbg.value(metadata i32 %5, metadata !321, metadata !DIExpression()), !dbg !316
  %6 = lshr i64 %0, 32, !dbg !322
  %7 = trunc i64 %6 to i32, !dbg !323
  call void @llvm.dbg.value(metadata i32 %7, metadata !324, metadata !DIExpression()), !dbg !316
  %8 = trunc i64 %1 to i32, !dbg !325
  call void @llvm.dbg.value(metadata i32 %8, metadata !326, metadata !DIExpression()), !dbg !316
  %9 = lshr i64 %1, 32, !dbg !327
  %10 = trunc i64 %9 to i32, !dbg !328
  call void @llvm.dbg.value(metadata i32 %10, metadata !329, metadata !DIExpression()), !dbg !316
  %11 = zext i32 %5 to i64, !dbg !330
  %12 = zext i32 %8 to i64, !dbg !331
  %13 = mul i64 %11, %12, !dbg !332
  call void @llvm.dbg.value(metadata i64 %13, metadata !333, metadata !DIExpression()), !dbg !316
  %14 = zext i32 %5 to i64, !dbg !334
  %15 = zext i32 %10 to i64, !dbg !335
  %16 = mul i64 %14, %15, !dbg !336
  call void @llvm.dbg.value(metadata i64 %16, metadata !337, metadata !DIExpression()), !dbg !316
  %17 = zext i32 %7 to i64, !dbg !338
  %18 = zext i32 %8 to i64, !dbg !339
  %19 = mul i64 %17, %18, !dbg !340
  call void @llvm.dbg.value(metadata i64 %19, metadata !341, metadata !DIExpression()), !dbg !316
  %20 = zext i32 %7 to i64, !dbg !342
  %21 = zext i32 %10 to i64, !dbg !343
  %22 = mul i64 %20, %21, !dbg !344
  call void @llvm.dbg.value(metadata i64 %22, metadata !345, metadata !DIExpression()), !dbg !316
  %23 = add i64 %16, %19, !dbg !346
  call void @llvm.dbg.value(metadata i64 %23, metadata !337, metadata !DIExpression()), !dbg !316
  %24 = icmp ult i64 %23, %19, !dbg !347
  %25 = zext i1 %24 to i64, !dbg !348
  %26 = shl i64 %25, 32, !dbg !349
  %27 = lshr i64 %23, 32, !dbg !350
  %28 = add i64 %26, %27, !dbg !351
  %29 = add i64 %22, %28, !dbg !352
  call void @llvm.dbg.value(metadata i64 %29, metadata !345, metadata !DIExpression()), !dbg !316
  %30 = shl i64 %23, 32, !dbg !353
  call void @llvm.dbg.value(metadata i64 %30, metadata !337, metadata !DIExpression()), !dbg !316
  %31 = add i64 %13, %30, !dbg !354
  call void @llvm.dbg.value(metadata i64 %31, metadata !333, metadata !DIExpression()), !dbg !316
  %32 = icmp ult i64 %31, %30, !dbg !355
  %33 = zext i1 %32 to i64, !dbg !356
  %34 = add i64 %29, %33, !dbg !357
  call void @llvm.dbg.value(metadata i64 %34, metadata !345, metadata !DIExpression()), !dbg !316
  store i64 %31, i64* %3, align 8, !dbg !358
  store i64 %34, i64* %2, align 8, !dbg !359
  ret void, !dbg !360
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z11float_raisei(i32 %0) #0 !dbg !361 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !365, metadata !DIExpression()), !dbg !366
  %2 = load i32, i32* @float_exception_flags, align 4, !dbg !367
  %3 = or i32 %2, %0, !dbg !367
  store i32 %3, i32* @float_exception_flags, align 4, !dbg !367
  ret void, !dbg !368
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local i32 @_Z14float64_is_nany(i64 %0) #0 !dbg !369 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !375, metadata !DIExpression()), !dbg !376
  %2 = shl i64 %0, 1, !dbg !377
  %3 = icmp ult i64 -9007199254740992, %2, !dbg !378
  %4 = zext i1 %3 to i32, !dbg !379
  ret i32 %4, !dbg !380
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local i32 @_Z24float64_is_signaling_nany(i64 %0) #0 !dbg !381 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !382, metadata !DIExpression()), !dbg !383
  %2 = lshr i64 %0, 51, !dbg !384
  %3 = and i64 %2, 4095, !dbg !385
  %4 = icmp eq i64 %3, 4094, !dbg !386
  br i1 %4, label %5, label %8, !dbg !387

5:                                                ; preds = %1
  %6 = and i64 %0, 2251799813685247, !dbg !388
  %7 = icmp ne i64 %6, 0, !dbg !389
  br label %8

8:                                                ; preds = %5, %1
  %9 = phi i1 [ false, %1 ], [ %7, %5 ], !dbg !383
  %10 = zext i1 %9 to i32, !dbg !390
  ret i32 %10, !dbg !391
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i64 @extractFloat64Frac(i64 %0) #2 !dbg !392 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !395, metadata !DIExpression()), !dbg !396
  %2 = and i64 %0, 4503599627370495, !dbg !397
  ret i64 %2, !dbg !398
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i32 @extractFloat64Exp(i64 %0) #2 !dbg !399 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !402, metadata !DIExpression()), !dbg !403
  %2 = lshr i64 %0, 52, !dbg !404
  %3 = and i64 %2, 2047, !dbg !405
  %4 = trunc i64 %3 to i32, !dbg !406
  ret i32 %4, !dbg !407
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i32 @extractFloat64Sign(i64 %0) #2 !dbg !408 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !409, metadata !DIExpression()), !dbg !410
  %2 = lshr i64 %0, 63, !dbg !411
  %3 = trunc i64 %2 to i32, !dbg !412
  ret i32 %3, !dbg !413
}

; Function Attrs: mustprogress noinline uwtable
define dso_local void @normalizeFloat64Subnormal(i64 %0, i32* %1, i64* %2) #3 !dbg !414 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !418, metadata !DIExpression()), !dbg !419
  call void @llvm.dbg.value(metadata i32* %1, metadata !420, metadata !DIExpression()), !dbg !419
  call void @llvm.dbg.value(metadata i64* %2, metadata !421, metadata !DIExpression()), !dbg !419
  %4 = call i32 @_ZL19countLeadingZeros64y(i64 %0), !dbg !422
  %5 = sub nsw i32 %4, 11, !dbg !423
  call void @llvm.dbg.value(metadata i32 %5, metadata !424, metadata !DIExpression()), !dbg !419
  %6 = zext i32 %5 to i64, !dbg !425
  %7 = shl i64 %0, %6, !dbg !425
  store i64 %7, i64* %2, align 8, !dbg !426
  %8 = sub nsw i32 1, %5, !dbg !427
  store i32 %8, i32* %1, align 4, !dbg !428
  ret void, !dbg !429
}

; Function Attrs: mustprogress noinline uwtable
define internal i32 @_ZL19countLeadingZeros64y(i64 %0) #3 !dbg !430 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !433, metadata !DIExpression()), !dbg !434
  call void @llvm.dbg.value(metadata i32 0, metadata !435, metadata !DIExpression()), !dbg !434
  %2 = icmp ult i64 %0, 4294967296, !dbg !436
  br i1 %2, label %3, label %5, !dbg !438

3:                                                ; preds = %1
  %4 = add nsw i32 0, 32, !dbg !439
  call void @llvm.dbg.value(metadata i32 %4, metadata !435, metadata !DIExpression()), !dbg !434
  br label %7, !dbg !441

5:                                                ; preds = %1
  %6 = lshr i64 %0, 32, !dbg !442
  call void @llvm.dbg.value(metadata i64 %6, metadata !433, metadata !DIExpression()), !dbg !434
  br label %7

7:                                                ; preds = %5, %3
  %.01 = phi i32 [ %4, %3 ], [ 0, %5 ], !dbg !434
  %.0 = phi i64 [ %0, %3 ], [ %6, %5 ]
  call void @llvm.dbg.value(metadata i64 %.0, metadata !433, metadata !DIExpression()), !dbg !434
  call void @llvm.dbg.value(metadata i32 %.01, metadata !435, metadata !DIExpression()), !dbg !434
  %8 = trunc i64 %.0 to i32, !dbg !444
  %9 = call i32 @_ZL19countLeadingZeros32j(i32 %8), !dbg !445
  %10 = add nsw i32 %.01, %9, !dbg !446
  call void @llvm.dbg.value(metadata i32 %10, metadata !435, metadata !DIExpression()), !dbg !434
  ret i32 %10, !dbg !447
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i64 @packFloat64(i32 %0, i32 %1, i64 %2) #2 !dbg !448 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !451, metadata !DIExpression()), !dbg !452
  call void @llvm.dbg.value(metadata i32 %1, metadata !453, metadata !DIExpression()), !dbg !452
  call void @llvm.dbg.value(metadata i64 %2, metadata !454, metadata !DIExpression()), !dbg !452
  %4 = sext i32 %0 to i64, !dbg !455
  %5 = shl i64 %4, 63, !dbg !456
  %6 = sext i32 %1 to i64, !dbg !457
  %7 = shl i64 %6, 52, !dbg !458
  %8 = add i64 %5, %7, !dbg !459
  %9 = add i64 %8, %2, !dbg !460
  ret i64 %9, !dbg !461
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local i64 @roundAndPackFloat64(i32 %0, i32 %1, i64 %2) #0 !dbg !462 {
  %4 = alloca i64, align 8
  call void @llvm.dbg.value(metadata i32 %0, metadata !463, metadata !DIExpression()), !dbg !464
  call void @llvm.dbg.value(metadata i32 %1, metadata !465, metadata !DIExpression()), !dbg !464
  store i64 %2, i64* %4, align 8
  call void @llvm.dbg.declare(metadata i64* %4, metadata !466, metadata !DIExpression()), !dbg !467
  %5 = load i32, i32* @float_rounding_mode, align 4, !dbg !468
  call void @llvm.dbg.value(metadata i32 %5, metadata !469, metadata !DIExpression()), !dbg !464
  %6 = icmp eq i32 %5, 0, !dbg !470
  %7 = zext i1 %6 to i32, !dbg !471
  call void @llvm.dbg.value(metadata i32 %7, metadata !472, metadata !DIExpression()), !dbg !464
  call void @llvm.dbg.value(metadata i32 512, metadata !473, metadata !DIExpression()), !dbg !464
  %8 = icmp ne i32 %7, 0, !dbg !474
  br i1 %8, label %24, label %9, !dbg !476

9:                                                ; preds = %3
  %10 = icmp eq i32 %5, 1, !dbg !477
  br i1 %10, label %11, label %12, !dbg !480

11:                                               ; preds = %9
  call void @llvm.dbg.value(metadata i32 0, metadata !473, metadata !DIExpression()), !dbg !464
  br label %23, !dbg !481

12:                                               ; preds = %9
  call void @llvm.dbg.value(metadata i32 1023, metadata !473, metadata !DIExpression()), !dbg !464
  %13 = icmp ne i32 %0, 0, !dbg !483
  br i1 %13, label %14, label %18, !dbg !486

14:                                               ; preds = %12
  %15 = icmp eq i32 %5, 2, !dbg !487
  br i1 %15, label %16, label %17, !dbg !490

16:                                               ; preds = %14
  call void @llvm.dbg.value(metadata i32 0, metadata !473, metadata !DIExpression()), !dbg !464
  br label %17, !dbg !491

17:                                               ; preds = %16, %14
  %.01 = phi i32 [ 0, %16 ], [ 1023, %14 ], !dbg !492
  call void @llvm.dbg.value(metadata i32 %.01, metadata !473, metadata !DIExpression()), !dbg !464
  br label %22, !dbg !493

18:                                               ; preds = %12
  %19 = icmp eq i32 %5, 3, !dbg !494
  br i1 %19, label %20, label %21, !dbg !497

20:                                               ; preds = %18
  call void @llvm.dbg.value(metadata i32 0, metadata !473, metadata !DIExpression()), !dbg !464
  br label %21, !dbg !498

21:                                               ; preds = %20, %18
  %.12 = phi i32 [ 0, %20 ], [ 1023, %18 ], !dbg !492
  call void @llvm.dbg.value(metadata i32 %.12, metadata !473, metadata !DIExpression()), !dbg !464
  br label %22

22:                                               ; preds = %21, %17
  %.2 = phi i32 [ %.01, %17 ], [ %.12, %21 ], !dbg !499
  call void @llvm.dbg.value(metadata i32 %.2, metadata !473, metadata !DIExpression()), !dbg !464
  br label %23

23:                                               ; preds = %22, %11
  %.3 = phi i32 [ 0, %11 ], [ %.2, %22 ], !dbg !500
  call void @llvm.dbg.value(metadata i32 %.3, metadata !473, metadata !DIExpression()), !dbg !464
  br label %24, !dbg !501

24:                                               ; preds = %23, %3
  %.4 = phi i32 [ 512, %3 ], [ %.3, %23 ], !dbg !464
  call void @llvm.dbg.value(metadata i32 %.4, metadata !473, metadata !DIExpression()), !dbg !464
  %25 = load i64, i64* %4, align 8, !dbg !502
  %26 = and i64 %25, 1023, !dbg !503
  %27 = trunc i64 %26 to i32, !dbg !502
  call void @llvm.dbg.value(metadata i32 %27, metadata !504, metadata !DIExpression()), !dbg !464
  %28 = trunc i32 %1 to i16, !dbg !505
  %29 = zext i16 %28 to i32, !dbg !507
  %30 = icmp sle i32 2045, %29, !dbg !508
  br i1 %30, label %31, label %64, !dbg !509

31:                                               ; preds = %24
  %32 = icmp slt i32 2045, %1, !dbg !510
  br i1 %32, label %40, label %33, !dbg !513

33:                                               ; preds = %31
  %34 = icmp eq i32 %1, 2045, !dbg !514
  br i1 %34, label %35, label %50, !dbg !515

35:                                               ; preds = %33
  %36 = load i64, i64* %4, align 8, !dbg !516
  %37 = sext i32 %.4 to i64, !dbg !517
  %38 = add i64 %36, %37, !dbg !518
  %39 = icmp slt i64 %38, 0, !dbg !519
  br i1 %39, label %40, label %50, !dbg !520

40:                                               ; preds = %35, %31
  call void @_Z11float_raisei(i32 9), !dbg !521
  call void @llvm.dbg.value(metadata i32 %0, metadata !451, metadata !DIExpression()), !dbg !523
  call void @llvm.dbg.value(metadata i32 2047, metadata !453, metadata !DIExpression()), !dbg !523
  call void @llvm.dbg.value(metadata i64 0, metadata !454, metadata !DIExpression()), !dbg !523
  %41 = sext i32 %0 to i64, !dbg !525
  %42 = shl i64 %41, 63, !dbg !526
  %43 = sext i32 2047 to i64, !dbg !527
  %44 = shl i64 %43, 52, !dbg !528
  %45 = add i64 %42, %44, !dbg !529
  %46 = add i64 %45, 0, !dbg !530
  %47 = icmp eq i32 %.4, 0, !dbg !531
  %48 = zext i1 %47 to i64, !dbg !532
  %49 = sub i64 %46, %48, !dbg !533
  br label %93, !dbg !534

50:                                               ; preds = %35, %33
  %51 = icmp slt i32 %1, 0, !dbg !535
  br i1 %51, label %52, label %63, !dbg !537

52:                                               ; preds = %50
  call void @llvm.dbg.value(metadata i32 1, metadata !538, metadata !DIExpression()), !dbg !464
  %53 = load i64, i64* %4, align 8, !dbg !539
  %54 = sub nsw i32 0, %1, !dbg !541
  call void @_Z19shift64RightJammingyiPy(i64 %53, i32 %54, i64* %4), !dbg !542
  call void @llvm.dbg.value(metadata i32 0, metadata !465, metadata !DIExpression()), !dbg !464
  %55 = load i64, i64* %4, align 8, !dbg !543
  %56 = and i64 %55, 1023, !dbg !544
  %57 = trunc i64 %56 to i32, !dbg !543
  call void @llvm.dbg.value(metadata i32 %57, metadata !504, metadata !DIExpression()), !dbg !464
  %58 = icmp ne i32 1, 0, !dbg !545
  br i1 %58, label %59, label %62, !dbg !547

59:                                               ; preds = %52
  %60 = icmp ne i32 %57, 0, !dbg !548
  br i1 %60, label %61, label %62, !dbg !549

61:                                               ; preds = %59
  call void @_Z11float_raisei(i32 4), !dbg !550
  br label %62, !dbg !550

62:                                               ; preds = %61, %59, %52
  br label %63, !dbg !551

63:                                               ; preds = %62, %50
  %.03 = phi i32 [ 0, %62 ], [ %1, %50 ]
  %.0 = phi i32 [ %57, %62 ], [ %27, %50 ], !dbg !464
  call void @llvm.dbg.value(metadata i32 %.0, metadata !504, metadata !DIExpression()), !dbg !464
  call void @llvm.dbg.value(metadata i32 %.03, metadata !465, metadata !DIExpression()), !dbg !464
  br label %64, !dbg !552

64:                                               ; preds = %63, %24
  %.14 = phi i32 [ %.03, %63 ], [ %1, %24 ]
  %.1 = phi i32 [ %.0, %63 ], [ %27, %24 ], !dbg !464
  call void @llvm.dbg.value(metadata i32 %.1, metadata !504, metadata !DIExpression()), !dbg !464
  call void @llvm.dbg.value(metadata i32 %.14, metadata !465, metadata !DIExpression()), !dbg !464
  %65 = icmp ne i32 %.1, 0, !dbg !553
  br i1 %65, label %66, label %69, !dbg !555

66:                                               ; preds = %64
  %67 = load i32, i32* @float_exception_flags, align 4, !dbg !556
  %68 = or i32 %67, 1, !dbg !556
  store i32 %68, i32* @float_exception_flags, align 4, !dbg !556
  br label %69, !dbg !557

69:                                               ; preds = %66, %64
  %70 = load i64, i64* %4, align 8, !dbg !558
  %71 = sext i32 %.4 to i64, !dbg !559
  %72 = add i64 %70, %71, !dbg !560
  %73 = lshr i64 %72, 10, !dbg !561
  store i64 %73, i64* %4, align 8, !dbg !562
  %74 = xor i32 %.1, 512, !dbg !563
  %75 = icmp eq i32 %74, 0, !dbg !564
  %76 = zext i1 %75 to i32, !dbg !565
  %77 = and i32 %76, %7, !dbg !566
  %78 = xor i32 %77, -1, !dbg !567
  %79 = sext i32 %78 to i64, !dbg !567
  %80 = load i64, i64* %4, align 8, !dbg !568
  %81 = and i64 %80, %79, !dbg !568
  store i64 %81, i64* %4, align 8, !dbg !568
  %82 = load i64, i64* %4, align 8, !dbg !569
  %83 = icmp eq i64 %82, 0, !dbg !571
  br i1 %83, label %84, label %85, !dbg !572

84:                                               ; preds = %69
  call void @llvm.dbg.value(metadata i32 0, metadata !465, metadata !DIExpression()), !dbg !464
  br label %85, !dbg !573

85:                                               ; preds = %84, %69
  %.25 = phi i32 [ 0, %84 ], [ %.14, %69 ], !dbg !464
  call void @llvm.dbg.value(metadata i32 %.25, metadata !465, metadata !DIExpression()), !dbg !464
  %86 = load i64, i64* %4, align 8, !dbg !574
  call void @llvm.dbg.value(metadata i32 %0, metadata !451, metadata !DIExpression()), !dbg !575
  call void @llvm.dbg.value(metadata i32 %.25, metadata !453, metadata !DIExpression()), !dbg !575
  call void @llvm.dbg.value(metadata i64 %86, metadata !454, metadata !DIExpression()), !dbg !575
  %87 = sext i32 %0 to i64, !dbg !577
  %88 = shl i64 %87, 63, !dbg !578
  %89 = sext i32 %.25 to i64, !dbg !579
  %90 = shl i64 %89, 52, !dbg !580
  %91 = add i64 %88, %90, !dbg !581
  %92 = add i64 %91, %86, !dbg !582
  br label %93, !dbg !583

93:                                               ; preds = %85, %40
  %.06 = phi i64 [ %49, %40 ], [ %92, %85 ], !dbg !464
  ret i64 %.06, !dbg !584
}

; Function Attrs: mustprogress noinline uwtable
define dso_local i64 @float64_mul(i64 %0, i64 %1) #3 !dbg !585 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i64, align 8
  call void @llvm.dbg.value(metadata i64 %0, metadata !590, metadata !DIExpression()), !dbg !591
  call void @llvm.dbg.value(metadata i64 %1, metadata !592, metadata !DIExpression()), !dbg !591
  call void @llvm.dbg.declare(metadata i32* %3, metadata !593, metadata !DIExpression()), !dbg !594
  call void @llvm.dbg.declare(metadata i32* %4, metadata !595, metadata !DIExpression()), !dbg !596
  call void @llvm.dbg.declare(metadata i64* %5, metadata !597, metadata !DIExpression()), !dbg !598
  call void @llvm.dbg.declare(metadata i64* %6, metadata !599, metadata !DIExpression()), !dbg !600
  call void @llvm.dbg.declare(metadata i64* %7, metadata !601, metadata !DIExpression()), !dbg !602
  call void @llvm.dbg.declare(metadata i64* %8, metadata !603, metadata !DIExpression()), !dbg !604
  call void @llvm.dbg.value(metadata i64 %0, metadata !395, metadata !DIExpression()), !dbg !605
  %9 = and i64 %0, 4503599627370495, !dbg !607
  store i64 %9, i64* %5, align 8, !dbg !608
  call void @llvm.dbg.value(metadata i64 %0, metadata !402, metadata !DIExpression()), !dbg !609
  %10 = lshr i64 %0, 52, !dbg !611
  %11 = and i64 %10, 2047, !dbg !612
  %12 = trunc i64 %11 to i32, !dbg !613
  store i32 %12, i32* %3, align 4, !dbg !614
  call void @llvm.dbg.value(metadata i64 %0, metadata !409, metadata !DIExpression()), !dbg !615
  %13 = lshr i64 %0, 63, !dbg !617
  %14 = trunc i64 %13 to i32, !dbg !618
  call void @llvm.dbg.value(metadata i32 %14, metadata !619, metadata !DIExpression()), !dbg !591
  call void @llvm.dbg.value(metadata i64 %1, metadata !395, metadata !DIExpression()), !dbg !620
  %15 = and i64 %1, 4503599627370495, !dbg !622
  store i64 %15, i64* %6, align 8, !dbg !623
  call void @llvm.dbg.value(metadata i64 %1, metadata !402, metadata !DIExpression()), !dbg !624
  %16 = lshr i64 %1, 52, !dbg !626
  %17 = and i64 %16, 2047, !dbg !627
  %18 = trunc i64 %17 to i32, !dbg !628
  store i32 %18, i32* %4, align 4, !dbg !629
  call void @llvm.dbg.value(metadata i64 %1, metadata !409, metadata !DIExpression()), !dbg !630
  %19 = lshr i64 %1, 63, !dbg !632
  %20 = trunc i64 %19 to i32, !dbg !633
  call void @llvm.dbg.value(metadata i32 %20, metadata !634, metadata !DIExpression()), !dbg !591
  %21 = xor i32 %14, %20, !dbg !635
  call void @llvm.dbg.value(metadata i32 %21, metadata !636, metadata !DIExpression()), !dbg !591
  %22 = load i32, i32* %3, align 4, !dbg !637
  %23 = icmp eq i32 %22, 2047, !dbg !639
  br i1 %23, label %24, label %49, !dbg !640

24:                                               ; preds = %2
  %25 = load i64, i64* %5, align 8, !dbg !641
  %26 = icmp ne i64 %25, 0, !dbg !641
  br i1 %26, label %33, label %27, !dbg !644

27:                                               ; preds = %24
  %28 = load i32, i32* %4, align 4, !dbg !645
  %29 = icmp eq i32 %28, 2047, !dbg !646
  br i1 %29, label %30, label %35, !dbg !647

30:                                               ; preds = %27
  %31 = load i64, i64* %6, align 8, !dbg !648
  %32 = icmp ne i64 %31, 0, !dbg !648
  br i1 %32, label %33, label %35, !dbg !649

33:                                               ; preds = %30, %24
  %34 = call i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1), !dbg !650
  br label %129, !dbg !651

35:                                               ; preds = %30, %27
  %36 = load i32, i32* %4, align 4, !dbg !652
  %37 = sext i32 %36 to i64, !dbg !652
  %38 = load i64, i64* %6, align 8, !dbg !654
  %39 = or i64 %37, %38, !dbg !655
  %40 = icmp eq i64 %39, 0, !dbg !656
  br i1 %40, label %41, label %42, !dbg !657

41:                                               ; preds = %35
  call void @_Z11float_raisei(i32 16), !dbg !658
  br label %129, !dbg !660

42:                                               ; preds = %35
  call void @llvm.dbg.value(metadata i32 %21, metadata !451, metadata !DIExpression()), !dbg !661
  call void @llvm.dbg.value(metadata i32 2047, metadata !453, metadata !DIExpression()), !dbg !661
  call void @llvm.dbg.value(metadata i64 0, metadata !454, metadata !DIExpression()), !dbg !661
  %43 = sext i32 %21 to i64, !dbg !663
  %44 = shl i64 %43, 63, !dbg !664
  %45 = sext i32 2047 to i64, !dbg !665
  %46 = shl i64 %45, 52, !dbg !666
  %47 = add i64 %44, %46, !dbg !667
  %48 = add i64 %47, 0, !dbg !668
  br label %129, !dbg !669

49:                                               ; preds = %2
  %50 = load i32, i32* %4, align 4, !dbg !670
  %51 = icmp eq i32 %50, 2047, !dbg !672
  br i1 %51, label %52, label %71, !dbg !673

52:                                               ; preds = %49
  %53 = load i64, i64* %6, align 8, !dbg !674
  %54 = icmp ne i64 %53, 0, !dbg !674
  br i1 %54, label %55, label %57, !dbg !677

55:                                               ; preds = %52
  %56 = call i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1), !dbg !678
  br label %129, !dbg !679

57:                                               ; preds = %52
  %58 = load i32, i32* %3, align 4, !dbg !680
  %59 = sext i32 %58 to i64, !dbg !680
  %60 = load i64, i64* %5, align 8, !dbg !682
  %61 = or i64 %59, %60, !dbg !683
  %62 = icmp eq i64 %61, 0, !dbg !684
  br i1 %62, label %63, label %64, !dbg !685

63:                                               ; preds = %57
  call void @_Z11float_raisei(i32 16), !dbg !686
  br label %129, !dbg !688

64:                                               ; preds = %57
  call void @llvm.dbg.value(metadata i32 %21, metadata !451, metadata !DIExpression()), !dbg !689
  call void @llvm.dbg.value(metadata i32 2047, metadata !453, metadata !DIExpression()), !dbg !689
  call void @llvm.dbg.value(metadata i64 0, metadata !454, metadata !DIExpression()), !dbg !689
  %65 = sext i32 %21 to i64, !dbg !691
  %66 = shl i64 %65, 63, !dbg !692
  %67 = sext i32 2047 to i64, !dbg !693
  %68 = shl i64 %67, 52, !dbg !694
  %69 = add i64 %66, %68, !dbg !695
  %70 = add i64 %69, 0, !dbg !696
  br label %129, !dbg !697

71:                                               ; preds = %49
  %72 = load i32, i32* %3, align 4, !dbg !698
  %73 = icmp eq i32 %72, 0, !dbg !700
  br i1 %73, label %74, label %86, !dbg !701

74:                                               ; preds = %71
  %75 = load i64, i64* %5, align 8, !dbg !702
  %76 = icmp eq i64 %75, 0, !dbg !705
  br i1 %76, label %77, label %84, !dbg !706

77:                                               ; preds = %74
  call void @llvm.dbg.value(metadata i32 %21, metadata !451, metadata !DIExpression()), !dbg !707
  call void @llvm.dbg.value(metadata i32 0, metadata !453, metadata !DIExpression()), !dbg !707
  call void @llvm.dbg.value(metadata i64 0, metadata !454, metadata !DIExpression()), !dbg !707
  %78 = sext i32 %21 to i64, !dbg !709
  %79 = shl i64 %78, 63, !dbg !710
  %80 = sext i32 0 to i64, !dbg !711
  %81 = shl i64 %80, 52, !dbg !712
  %82 = add i64 %79, %81, !dbg !713
  %83 = add i64 %82, 0, !dbg !714
  br label %129, !dbg !715

84:                                               ; preds = %74
  %85 = load i64, i64* %5, align 8, !dbg !716
  call void @normalizeFloat64Subnormal(i64 %85, i32* %3, i64* %5), !dbg !717
  br label %86, !dbg !718

86:                                               ; preds = %84, %71
  %87 = load i32, i32* %4, align 4, !dbg !719
  %88 = icmp eq i32 %87, 0, !dbg !721
  br i1 %88, label %89, label %101, !dbg !722

89:                                               ; preds = %86
  %90 = load i64, i64* %6, align 8, !dbg !723
  %91 = icmp eq i64 %90, 0, !dbg !726
  br i1 %91, label %92, label %99, !dbg !727

92:                                               ; preds = %89
  call void @llvm.dbg.value(metadata i32 %21, metadata !451, metadata !DIExpression()), !dbg !728
  call void @llvm.dbg.value(metadata i32 0, metadata !453, metadata !DIExpression()), !dbg !728
  call void @llvm.dbg.value(metadata i64 0, metadata !454, metadata !DIExpression()), !dbg !728
  %93 = sext i32 %21 to i64, !dbg !730
  %94 = shl i64 %93, 63, !dbg !731
  %95 = sext i32 0 to i64, !dbg !732
  %96 = shl i64 %95, 52, !dbg !733
  %97 = add i64 %94, %96, !dbg !734
  %98 = add i64 %97, 0, !dbg !735
  br label %129, !dbg !736

99:                                               ; preds = %89
  %100 = load i64, i64* %6, align 8, !dbg !737
  call void @normalizeFloat64Subnormal(i64 %100, i32* %4, i64* %6), !dbg !738
  br label %101, !dbg !739

101:                                              ; preds = %99, %86
  %102 = load i32, i32* %3, align 4, !dbg !740
  %103 = load i32, i32* %4, align 4, !dbg !741
  %104 = add nsw i32 %102, %103, !dbg !742
  %105 = sub nsw i32 %104, 1023, !dbg !743
  call void @llvm.dbg.value(metadata i32 %105, metadata !744, metadata !DIExpression()), !dbg !591
  %106 = load i64, i64* %5, align 8, !dbg !745
  %107 = or i64 %106, 4503599627370496, !dbg !746
  %108 = shl i64 %107, 10, !dbg !747
  store i64 %108, i64* %5, align 8, !dbg !748
  %109 = load i64, i64* %6, align 8, !dbg !749
  %110 = or i64 %109, 4503599627370496, !dbg !750
  %111 = shl i64 %110, 11, !dbg !751
  store i64 %111, i64* %6, align 8, !dbg !752
  %112 = load i64, i64* %5, align 8, !dbg !753
  %113 = load i64, i64* %6, align 8, !dbg !754
  call void @_Z10mul64To128yyPyS_(i64 %112, i64 %113, i64* %7, i64* %8), !dbg !755
  %114 = load i64, i64* %8, align 8, !dbg !756
  %115 = icmp ne i64 %114, 0, !dbg !757
  %116 = zext i1 %115 to i64, !dbg !758
  %117 = load i64, i64* %7, align 8, !dbg !759
  %118 = or i64 %117, %116, !dbg !759
  store i64 %118, i64* %7, align 8, !dbg !759
  %119 = load i64, i64* %7, align 8, !dbg !760
  %120 = shl i64 %119, 1, !dbg !762
  %121 = icmp sle i64 0, %120, !dbg !763
  br i1 %121, label %122, label %126, !dbg !764

122:                                              ; preds = %101
  %123 = load i64, i64* %7, align 8, !dbg !765
  %124 = shl i64 %123, 1, !dbg !765
  store i64 %124, i64* %7, align 8, !dbg !765
  %125 = add nsw i32 %105, -1, !dbg !767
  call void @llvm.dbg.value(metadata i32 %125, metadata !744, metadata !DIExpression()), !dbg !591
  br label %126, !dbg !768

126:                                              ; preds = %122, %101
  %.0 = phi i32 [ %125, %122 ], [ %105, %101 ], !dbg !591
  call void @llvm.dbg.value(metadata i32 %.0, metadata !744, metadata !DIExpression()), !dbg !591
  %127 = load i64, i64* %7, align 8, !dbg !769
  %128 = call i64 @roundAndPackFloat64(i32 %21, i32 %.0, i64 %127), !dbg !770
  br label %129, !dbg !771

129:                                              ; preds = %126, %92, %77, %64, %63, %55, %42, %41, %33
  %.01 = phi i64 [ %34, %33 ], [ 9223372036854775807, %41 ], [ %48, %42 ], [ %56, %55 ], [ 9223372036854775807, %63 ], [ %70, %64 ], [ %83, %77 ], [ %98, %92 ], [ %128, %126 ], !dbg !591
  ret i64 %.01, !dbg !772
}

; Function Attrs: mustprogress noinline nounwind uwtable
define internal i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1) #0 !dbg !773 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !776, metadata !DIExpression()), !dbg !777
  call void @llvm.dbg.value(metadata i64 %1, metadata !778, metadata !DIExpression()), !dbg !777
  %3 = call i32 @_Z14float64_is_nany(i64 %0), !dbg !779
  call void @llvm.dbg.value(metadata i32 %3, metadata !780, metadata !DIExpression()), !dbg !777
  %4 = call i32 @_Z24float64_is_signaling_nany(i64 %0), !dbg !781
  call void @llvm.dbg.value(metadata i32 %4, metadata !782, metadata !DIExpression()), !dbg !777
  %5 = call i32 @_Z14float64_is_nany(i64 %1), !dbg !783
  call void @llvm.dbg.value(metadata i32 %5, metadata !784, metadata !DIExpression()), !dbg !777
  %6 = call i32 @_Z24float64_is_signaling_nany(i64 %1), !dbg !785
  call void @llvm.dbg.value(metadata i32 %6, metadata !786, metadata !DIExpression()), !dbg !777
  %7 = or i64 %0, 2251799813685248, !dbg !787
  call void @llvm.dbg.value(metadata i64 %7, metadata !776, metadata !DIExpression()), !dbg !777
  %8 = or i64 %1, 2251799813685248, !dbg !788
  call void @llvm.dbg.value(metadata i64 %8, metadata !778, metadata !DIExpression()), !dbg !777
  %9 = or i32 %4, %6, !dbg !789
  %10 = icmp ne i32 %9, 0, !dbg !791
  br i1 %10, label %11, label %12, !dbg !792

11:                                               ; preds = %2
  call void @_Z11float_raisei(i32 16), !dbg !793
  br label %12, !dbg !793

12:                                               ; preds = %11, %2
  %13 = icmp ne i32 %6, 0, !dbg !794
  br i1 %13, label %14, label %15, !dbg !794

14:                                               ; preds = %12
  br label %26, !dbg !794

15:                                               ; preds = %12
  %16 = icmp ne i32 %4, 0, !dbg !795
  br i1 %16, label %17, label %18, !dbg !795

17:                                               ; preds = %15
  br label %24, !dbg !795

18:                                               ; preds = %15
  %19 = icmp ne i32 %5, 0, !dbg !796
  br i1 %19, label %20, label %21, !dbg !796

20:                                               ; preds = %18
  br label %22, !dbg !796

21:                                               ; preds = %18
  br label %22, !dbg !796

22:                                               ; preds = %21, %20
  %23 = phi i64 [ %8, %20 ], [ %7, %21 ], !dbg !796
  br label %24, !dbg !795

24:                                               ; preds = %22, %17
  %25 = phi i64 [ %7, %17 ], [ %23, %22 ], !dbg !795
  br label %26, !dbg !794

26:                                               ; preds = %24, %14
  %27 = phi i64 [ %8, %14 ], [ %25, %24 ], !dbg !794
  ret i64 %27, !dbg !797
}

; Function Attrs: mustprogress noinline nounwind uwtable
define internal i32 @_ZL19countLeadingZeros32j(i32 %0) #0 !dbg !20 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !798, metadata !DIExpression()), !dbg !799
  call void @llvm.dbg.value(metadata i32 0, metadata !800, metadata !DIExpression()), !dbg !799
  %2 = icmp ult i32 %0, 65536, !dbg !801
  br i1 %2, label %3, label %6, !dbg !803

3:                                                ; preds = %1
  %4 = add nsw i32 0, 16, !dbg !804
  call void @llvm.dbg.value(metadata i32 %4, metadata !800, metadata !DIExpression()), !dbg !799
  %5 = shl i32 %0, 16, !dbg !806
  call void @llvm.dbg.value(metadata i32 %5, metadata !798, metadata !DIExpression()), !dbg !799
  br label %6, !dbg !807

6:                                                ; preds = %3, %1
  %.01 = phi i32 [ %4, %3 ], [ 0, %1 ], !dbg !799
  %.0 = phi i32 [ %5, %3 ], [ %0, %1 ]
  call void @llvm.dbg.value(metadata i32 %.0, metadata !798, metadata !DIExpression()), !dbg !799
  call void @llvm.dbg.value(metadata i32 %.01, metadata !800, metadata !DIExpression()), !dbg !799
  %7 = icmp ult i32 %.0, 16777216, !dbg !808
  br i1 %7, label %8, label %11, !dbg !810

8:                                                ; preds = %6
  %9 = add nsw i32 %.01, 8, !dbg !811
  call void @llvm.dbg.value(metadata i32 %9, metadata !800, metadata !DIExpression()), !dbg !799
  %10 = shl i32 %.0, 8, !dbg !813
  call void @llvm.dbg.value(metadata i32 %10, metadata !798, metadata !DIExpression()), !dbg !799
  br label %11, !dbg !814

11:                                               ; preds = %8, %6
  %.12 = phi i32 [ %9, %8 ], [ %.01, %6 ], !dbg !799
  %.1 = phi i32 [ %10, %8 ], [ %.0, %6 ], !dbg !799
  call void @llvm.dbg.value(metadata i32 %.1, metadata !798, metadata !DIExpression()), !dbg !799
  call void @llvm.dbg.value(metadata i32 %.12, metadata !800, metadata !DIExpression()), !dbg !799
  %12 = lshr i32 %.1, 24, !dbg !815
  %13 = zext i32 %12 to i64, !dbg !816
  %14 = getelementptr inbounds [256 x i32], [256 x i32]* bitcast (<{ [128 x i32], [128 x i32] }>* @_ZZL19countLeadingZeros32jE21countLeadingZerosHigh to [256 x i32]*), i64 0, i64 %13, !dbg !816
  %15 = load i32, i32* %14, align 4, !dbg !816
  %16 = add nsw i32 %.12, %15, !dbg !817
  call void @llvm.dbg.value(metadata i32 %16, metadata !800, metadata !DIExpression()), !dbg !799
  ret i32 %16, !dbg !818
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { alwaysinline mustprogress nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!272, !273, !274, !275, !276}
!llvm.ident = !{!277}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "float_rounding_mode", scope: !2, file: !3, line: 61, type: !16, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !3, producer: "Ubuntu clang version 13.0.1-2ubuntu2.2", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !13, imports: !30, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "dfmul/float64_mul.cpp", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!4 = !{}
!5 = !{!6, !9, !11}
!6 = !DIDerivedType(tag: DW_TAG_typedef, name: "bits64", file: !7, line: 70, baseType: !8)
!7 = !DIFile(filename: "dfmul/include/SPARC-GCC.h", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!8 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!9 = !DIDerivedType(tag: DW_TAG_typedef, name: "bits16", file: !7, line: 68, baseType: !10)
!10 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!11 = !DIDerivedType(tag: DW_TAG_typedef, name: "sbits64", file: !7, line: 71, baseType: !12)
!12 = !DIBasicType(name: "long long int", size: 64, encoding: DW_ATE_signed)
!13 = !{!0, !14, !18}
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "float_exception_flags", scope: !2, file: !3, line: 62, type: !16, isLocal: false, isDefinition: true)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8", file: !7, line: 59, baseType: !17)
!17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(name: "countLeadingZerosHigh", scope: !20, file: !21, line: 118, type: !26, isLocal: true, isDefinition: true)
!20 = distinct !DISubprogram(name: "countLeadingZeros32", linkageName: "_ZL19countLeadingZeros32j", scope: !21, file: !21, line: 116, type: !22, scopeLine: 117, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!21 = !DIFile(filename: "dfmul/include/softfloat-macros", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!22 = !DISubroutineType(types: !23)
!23 = !{!16, !24}
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "bits32", file: !7, line: 69, baseType: !25)
!25 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!26 = !DICompositeType(tag: DW_TAG_array_type, baseType: !27, size: 8192, elements: !28)
!27 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !16)
!28 = !{!29}
!29 = !DISubrange(count: 256)
!30 = !{!31, !38, !42, !49, !53, !58, !60, !68, !72, !76, !90, !94, !98, !102, !106, !111, !115, !119, !123, !127, !135, !139, !143, !145, !149, !153, !157, !163, !167, !171, !173, !181, !185, !192, !194, !198, !202, !206, !210, !214, !219, !224, !225, !226, !227, !229, !230, !231, !232, !233, !234, !235, !237, !238, !239, !240, !241, !242, !243, !248, !249, !250, !251, !252, !253, !254, !255, !256, !257, !258, !259, !260, !261, !262, !263, !264, !265, !266, !267, !268, !269, !270, !271}
!31 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !33, file: !37, line: 52)
!32 = !DINamespace(name: "std", scope: null)
!33 = !DISubprogram(name: "abs", scope: !34, file: !34, line: 848, type: !35, flags: DIFlagPrototyped, spFlags: 0)
!34 = !DIFile(filename: "/usr/include/stdlib.h", directory: "")
!35 = !DISubroutineType(types: !36)
!36 = !{!17, !17}
!37 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/bits/std_abs.h", directory: "")
!38 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !39, file: !41, line: 127)
!39 = !DIDerivedType(tag: DW_TAG_typedef, name: "div_t", file: !34, line: 63, baseType: !40)
!40 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !34, line: 59, size: 64, flags: DIFlagFwdDecl, identifier: "_ZTS5div_t")
!41 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/cstdlib", directory: "")
!42 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !43, file: !41, line: 128)
!43 = !DIDerivedType(tag: DW_TAG_typedef, name: "ldiv_t", file: !34, line: 71, baseType: !44)
!44 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !34, line: 67, size: 128, flags: DIFlagTypePassByValue, elements: !45, identifier: "_ZTS6ldiv_t")
!45 = !{!46, !48}
!46 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !44, file: !34, line: 69, baseType: !47, size: 64)
!47 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!48 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !44, file: !34, line: 70, baseType: !47, size: 64, offset: 64)
!49 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !50, file: !41, line: 130)
!50 = !DISubprogram(name: "abort", scope: !34, file: !34, line: 598, type: !51, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!51 = !DISubroutineType(types: !52)
!52 = !{null}
!53 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !54, file: !41, line: 134)
!54 = !DISubprogram(name: "atexit", scope: !34, file: !34, line: 602, type: !55, flags: DIFlagPrototyped, spFlags: 0)
!55 = !DISubroutineType(types: !56)
!56 = !{!17, !57}
!57 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !51, size: 64)
!58 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !59, file: !41, line: 137)
!59 = !DISubprogram(name: "at_quick_exit", scope: !34, file: !34, line: 607, type: !55, flags: DIFlagPrototyped, spFlags: 0)
!60 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !61, file: !41, line: 140)
!61 = !DISubprogram(name: "atof", scope: !34, file: !34, line: 102, type: !62, flags: DIFlagPrototyped, spFlags: 0)
!62 = !DISubroutineType(types: !63)
!63 = !{!64, !65}
!64 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!65 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !66, size: 64)
!66 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !67)
!67 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!68 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !69, file: !41, line: 141)
!69 = !DISubprogram(name: "atoi", scope: !34, file: !34, line: 105, type: !70, flags: DIFlagPrototyped, spFlags: 0)
!70 = !DISubroutineType(types: !71)
!71 = !{!17, !65}
!72 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !73, file: !41, line: 142)
!73 = !DISubprogram(name: "atol", scope: !34, file: !34, line: 108, type: !74, flags: DIFlagPrototyped, spFlags: 0)
!74 = !DISubroutineType(types: !75)
!75 = !{!47, !65}
!76 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !77, file: !41, line: 143)
!77 = !DISubprogram(name: "bsearch", scope: !34, file: !34, line: 828, type: !78, flags: DIFlagPrototyped, spFlags: 0)
!78 = !DISubroutineType(types: !79)
!79 = !{!80, !81, !81, !83, !83, !86}
!80 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!81 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !82, size: 64)
!82 = !DIDerivedType(tag: DW_TAG_const_type, baseType: null)
!83 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !84, line: 46, baseType: !85)
!84 = !DIFile(filename: "/usr/lib/llvm-13/lib/clang/13.0.1/include/stddef.h", directory: "")
!85 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!86 = !DIDerivedType(tag: DW_TAG_typedef, name: "__compar_fn_t", file: !34, line: 816, baseType: !87)
!87 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !88, size: 64)
!88 = !DISubroutineType(types: !89)
!89 = !{!17, !81, !81}
!90 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !91, file: !41, line: 144)
!91 = !DISubprogram(name: "calloc", scope: !34, file: !34, line: 543, type: !92, flags: DIFlagPrototyped, spFlags: 0)
!92 = !DISubroutineType(types: !93)
!93 = !{!80, !83, !83}
!94 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !95, file: !41, line: 145)
!95 = !DISubprogram(name: "div", scope: !34, file: !34, line: 860, type: !96, flags: DIFlagPrototyped, spFlags: 0)
!96 = !DISubroutineType(types: !97)
!97 = !{!39, !17, !17}
!98 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !99, file: !41, line: 146)
!99 = !DISubprogram(name: "exit", scope: !34, file: !34, line: 624, type: !100, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!100 = !DISubroutineType(types: !101)
!101 = !{null, !17}
!102 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !103, file: !41, line: 147)
!103 = !DISubprogram(name: "free", scope: !34, file: !34, line: 555, type: !104, flags: DIFlagPrototyped, spFlags: 0)
!104 = !DISubroutineType(types: !105)
!105 = !{null, !80}
!106 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !107, file: !41, line: 148)
!107 = !DISubprogram(name: "getenv", scope: !34, file: !34, line: 641, type: !108, flags: DIFlagPrototyped, spFlags: 0)
!108 = !DISubroutineType(types: !109)
!109 = !{!110, !65}
!110 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !67, size: 64)
!111 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !112, file: !41, line: 149)
!112 = !DISubprogram(name: "labs", scope: !34, file: !34, line: 849, type: !113, flags: DIFlagPrototyped, spFlags: 0)
!113 = !DISubroutineType(types: !114)
!114 = !{!47, !47}
!115 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !116, file: !41, line: 150)
!116 = !DISubprogram(name: "ldiv", scope: !34, file: !34, line: 862, type: !117, flags: DIFlagPrototyped, spFlags: 0)
!117 = !DISubroutineType(types: !118)
!118 = !{!43, !47, !47}
!119 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !120, file: !41, line: 151)
!120 = !DISubprogram(name: "malloc", scope: !34, file: !34, line: 540, type: !121, flags: DIFlagPrototyped, spFlags: 0)
!121 = !DISubroutineType(types: !122)
!122 = !{!80, !83}
!123 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !124, file: !41, line: 153)
!124 = !DISubprogram(name: "mblen", scope: !34, file: !34, line: 930, type: !125, flags: DIFlagPrototyped, spFlags: 0)
!125 = !DISubroutineType(types: !126)
!126 = !{!17, !65, !83}
!127 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !128, file: !41, line: 154)
!128 = !DISubprogram(name: "mbstowcs", scope: !34, file: !34, line: 941, type: !129, flags: DIFlagPrototyped, spFlags: 0)
!129 = !DISubroutineType(types: !130)
!130 = !{!83, !131, !134, !83}
!131 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !132)
!132 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !133, size: 64)
!133 = !DIBasicType(name: "wchar_t", size: 32, encoding: DW_ATE_signed)
!134 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !65)
!135 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !136, file: !41, line: 155)
!136 = !DISubprogram(name: "mbtowc", scope: !34, file: !34, line: 933, type: !137, flags: DIFlagPrototyped, spFlags: 0)
!137 = !DISubroutineType(types: !138)
!138 = !{!17, !131, !134, !83}
!139 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !140, file: !41, line: 157)
!140 = !DISubprogram(name: "qsort", scope: !34, file: !34, line: 838, type: !141, flags: DIFlagPrototyped, spFlags: 0)
!141 = !DISubroutineType(types: !142)
!142 = !{null, !80, !83, !83, !86}
!143 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !144, file: !41, line: 160)
!144 = !DISubprogram(name: "quick_exit", scope: !34, file: !34, line: 630, type: !100, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!145 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !146, file: !41, line: 163)
!146 = !DISubprogram(name: "rand", scope: !34, file: !34, line: 454, type: !147, flags: DIFlagPrototyped, spFlags: 0)
!147 = !DISubroutineType(types: !148)
!148 = !{!17}
!149 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !150, file: !41, line: 164)
!150 = !DISubprogram(name: "realloc", scope: !34, file: !34, line: 551, type: !151, flags: DIFlagPrototyped, spFlags: 0)
!151 = !DISubroutineType(types: !152)
!152 = !{!80, !80, !83}
!153 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !154, file: !41, line: 165)
!154 = !DISubprogram(name: "srand", scope: !34, file: !34, line: 456, type: !155, flags: DIFlagPrototyped, spFlags: 0)
!155 = !DISubroutineType(types: !156)
!156 = !{null, !25}
!157 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !158, file: !41, line: 166)
!158 = !DISubprogram(name: "strtod", scope: !34, file: !34, line: 118, type: !159, flags: DIFlagPrototyped, spFlags: 0)
!159 = !DISubroutineType(types: !160)
!160 = !{!64, !134, !161}
!161 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !162)
!162 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !110, size: 64)
!163 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !164, file: !41, line: 167)
!164 = !DISubprogram(name: "strtol", scope: !34, file: !34, line: 177, type: !165, flags: DIFlagPrototyped, spFlags: 0)
!165 = !DISubroutineType(types: !166)
!166 = !{!47, !134, !161, !17}
!167 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !168, file: !41, line: 168)
!168 = !DISubprogram(name: "strtoul", scope: !34, file: !34, line: 181, type: !169, flags: DIFlagPrototyped, spFlags: 0)
!169 = !DISubroutineType(types: !170)
!170 = !{!85, !134, !161, !17}
!171 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !172, file: !41, line: 169)
!172 = !DISubprogram(name: "system", scope: !34, file: !34, line: 791, type: !70, flags: DIFlagPrototyped, spFlags: 0)
!173 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !174, file: !41, line: 171)
!174 = !DISubprogram(name: "wcstombs", scope: !34, file: !34, line: 945, type: !175, flags: DIFlagPrototyped, spFlags: 0)
!175 = !DISubroutineType(types: !176)
!176 = !{!83, !177, !178, !83}
!177 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !110)
!178 = !DIDerivedType(tag: DW_TAG_restrict_type, baseType: !179)
!179 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !180, size: 64)
!180 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !133)
!181 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !182, file: !41, line: 172)
!182 = !DISubprogram(name: "wctomb", scope: !34, file: !34, line: 937, type: !183, flags: DIFlagPrototyped, spFlags: 0)
!183 = !DISubroutineType(types: !184)
!184 = !{!17, !110, !133}
!185 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !187, file: !41, line: 200)
!186 = !DINamespace(name: "__gnu_cxx", scope: null)
!187 = !DIDerivedType(tag: DW_TAG_typedef, name: "lldiv_t", file: !34, line: 81, baseType: !188)
!188 = distinct !DICompositeType(tag: DW_TAG_structure_type, file: !34, line: 77, size: 128, flags: DIFlagTypePassByValue, elements: !189, identifier: "_ZTS7lldiv_t")
!189 = !{!190, !191}
!190 = !DIDerivedType(tag: DW_TAG_member, name: "quot", scope: !188, file: !34, line: 79, baseType: !12, size: 64)
!191 = !DIDerivedType(tag: DW_TAG_member, name: "rem", scope: !188, file: !34, line: 80, baseType: !12, size: 64, offset: 64)
!192 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !193, file: !41, line: 206)
!193 = !DISubprogram(name: "_Exit", scope: !34, file: !34, line: 636, type: !100, flags: DIFlagPrototyped | DIFlagNoReturn, spFlags: 0)
!194 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !195, file: !41, line: 210)
!195 = !DISubprogram(name: "llabs", scope: !34, file: !34, line: 852, type: !196, flags: DIFlagPrototyped, spFlags: 0)
!196 = !DISubroutineType(types: !197)
!197 = !{!12, !12}
!198 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !199, file: !41, line: 216)
!199 = !DISubprogram(name: "lldiv", scope: !34, file: !34, line: 866, type: !200, flags: DIFlagPrototyped, spFlags: 0)
!200 = !DISubroutineType(types: !201)
!201 = !{!187, !12, !12}
!202 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !203, file: !41, line: 227)
!203 = !DISubprogram(name: "atoll", scope: !34, file: !34, line: 113, type: !204, flags: DIFlagPrototyped, spFlags: 0)
!204 = !DISubroutineType(types: !205)
!205 = !{!12, !65}
!206 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !207, file: !41, line: 228)
!207 = !DISubprogram(name: "strtoll", scope: !34, file: !34, line: 201, type: !208, flags: DIFlagPrototyped, spFlags: 0)
!208 = !DISubroutineType(types: !209)
!209 = !{!12, !134, !161, !17}
!210 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !211, file: !41, line: 229)
!211 = !DISubprogram(name: "strtoull", scope: !34, file: !34, line: 206, type: !212, flags: DIFlagPrototyped, spFlags: 0)
!212 = !DISubroutineType(types: !213)
!213 = !{!8, !134, !161, !17}
!214 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !215, file: !41, line: 231)
!215 = !DISubprogram(name: "strtof", scope: !34, file: !34, line: 124, type: !216, flags: DIFlagPrototyped, spFlags: 0)
!216 = !DISubroutineType(types: !217)
!217 = !{!218, !134, !161}
!218 = !DIBasicType(name: "float", size: 32, encoding: DW_ATE_float)
!219 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !186, entity: !220, file: !41, line: 232)
!220 = !DISubprogram(name: "strtold", scope: !34, file: !34, line: 127, type: !221, flags: DIFlagPrototyped, spFlags: 0)
!221 = !DISubroutineType(types: !222)
!222 = !{!223, !134, !161}
!223 = !DIBasicType(name: "long double", size: 128, encoding: DW_ATE_float)
!224 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !187, file: !41, line: 240)
!225 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !193, file: !41, line: 242)
!226 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !195, file: !41, line: 244)
!227 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !228, file: !41, line: 245)
!228 = !DISubprogram(name: "div", linkageName: "_ZN9__gnu_cxx3divExx", scope: !186, file: !41, line: 213, type: !200, flags: DIFlagPrototyped, spFlags: 0)
!229 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !199, file: !41, line: 246)
!230 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !203, file: !41, line: 248)
!231 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !215, file: !41, line: 249)
!232 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !207, file: !41, line: 250)
!233 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !211, file: !41, line: 251)
!234 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !32, entity: !220, file: !41, line: 252)
!235 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !50, file: !236, line: 38)
!236 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../include/c++/11/stdlib.h", directory: "")
!237 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !54, file: !236, line: 39)
!238 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !99, file: !236, line: 40)
!239 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !59, file: !236, line: 43)
!240 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !144, file: !236, line: 46)
!241 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !39, file: !236, line: 51)
!242 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !43, file: !236, line: 52)
!243 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !244, file: !236, line: 54)
!244 = !DISubprogram(name: "abs", linkageName: "_ZSt3absg", scope: !32, file: !37, line: 103, type: !245, flags: DIFlagPrototyped, spFlags: 0)
!245 = !DISubroutineType(types: !246)
!246 = !{!247, !247}
!247 = !DIBasicType(name: "__float128", size: 128, encoding: DW_ATE_float)
!248 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !61, file: !236, line: 55)
!249 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !69, file: !236, line: 56)
!250 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !73, file: !236, line: 57)
!251 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !77, file: !236, line: 58)
!252 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !91, file: !236, line: 59)
!253 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !228, file: !236, line: 60)
!254 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !103, file: !236, line: 61)
!255 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !107, file: !236, line: 62)
!256 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !112, file: !236, line: 63)
!257 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !116, file: !236, line: 64)
!258 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !120, file: !236, line: 65)
!259 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !124, file: !236, line: 67)
!260 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !128, file: !236, line: 68)
!261 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !136, file: !236, line: 69)
!262 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !140, file: !236, line: 71)
!263 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !146, file: !236, line: 72)
!264 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !150, file: !236, line: 73)
!265 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !154, file: !236, line: 74)
!266 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !158, file: !236, line: 75)
!267 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !164, file: !236, line: 76)
!268 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !168, file: !236, line: 77)
!269 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !172, file: !236, line: 78)
!270 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !174, file: !236, line: 80)
!271 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !2, entity: !182, file: !236, line: 81)
!272 = !{i32 7, !"Dwarf Version", i32 4}
!273 = !{i32 2, !"Debug Info Version", i32 3}
!274 = !{i32 1, !"wchar_size", i32 4}
!275 = !{i32 7, !"uwtable", i32 1}
!276 = !{i32 7, !"frame-pointer", i32 2}
!277 = !{!"Ubuntu clang version 13.0.1-2ubuntu2.2"}
!278 = distinct !DISubprogram(name: "shift64RightJamming", linkageName: "_Z19shift64RightJammingyiPy", scope: !21, file: !21, line: 60, type: !279, scopeLine: 61, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!279 = !DISubroutineType(types: !280)
!280 = !{null, !6, !281, !282}
!281 = !DIDerivedType(tag: DW_TAG_typedef, name: "int16", file: !7, line: 60, baseType: !17)
!282 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !6, size: 64)
!283 = !DILocalVariable(name: "a", arg: 1, scope: !278, file: !21, line: 60, type: !6)
!284 = !DILocation(line: 0, scope: !278)
!285 = !DILocalVariable(name: "count", arg: 2, scope: !278, file: !21, line: 60, type: !281)
!286 = !DILocalVariable(name: "zPtr", arg: 3, scope: !278, file: !21, line: 60, type: !282)
!287 = !DILocation(line: 64, column: 13, scope: !288)
!288 = distinct !DILexicalBlock(scope: !278, file: !21, line: 64, column: 7)
!289 = !DILocation(line: 64, column: 7, scope: !278)
!290 = !DILocalVariable(name: "z", scope: !278, file: !21, line: 62, type: !6)
!291 = !DILocation(line: 67, column: 5, scope: !292)
!292 = distinct !DILexicalBlock(scope: !288, file: !21, line: 65, column: 5)
!293 = !DILocation(line: 68, column: 18, scope: !294)
!294 = distinct !DILexicalBlock(scope: !288, file: !21, line: 68, column: 12)
!295 = !DILocation(line: 68, column: 12, scope: !288)
!296 = !DILocation(line: 70, column: 14, scope: !297)
!297 = distinct !DILexicalBlock(scope: !294, file: !21, line: 69, column: 5)
!298 = !DILocation(line: 70, column: 35, scope: !297)
!299 = !DILocation(line: 70, column: 43, scope: !297)
!300 = !DILocation(line: 70, column: 30, scope: !297)
!301 = !DILocation(line: 70, column: 50, scope: !297)
!302 = !DILocation(line: 70, column: 26, scope: !297)
!303 = !DILocation(line: 70, column: 24, scope: !297)
!304 = !DILocation(line: 71, column: 5, scope: !297)
!305 = !DILocation(line: 74, column: 14, scope: !306)
!306 = distinct !DILexicalBlock(scope: !294, file: !21, line: 73, column: 5)
!307 = !DILocation(line: 74, column: 11, scope: !306)
!308 = !DILocation(line: 0, scope: !294)
!309 = !DILocation(line: 0, scope: !288)
!310 = !DILocation(line: 76, column: 9, scope: !278)
!311 = !DILocation(line: 78, column: 1, scope: !278)
!312 = distinct !DISubprogram(name: "mul64To128", linkageName: "_Z10mul64To128yyPyS_", scope: !21, file: !21, line: 87, type: !313, scopeLine: 88, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!313 = !DISubroutineType(types: !314)
!314 = !{null, !6, !6, !282, !282}
!315 = !DILocalVariable(name: "a", arg: 1, scope: !312, file: !21, line: 87, type: !6)
!316 = !DILocation(line: 0, scope: !312)
!317 = !DILocalVariable(name: "b", arg: 2, scope: !312, file: !21, line: 87, type: !6)
!318 = !DILocalVariable(name: "z0Ptr", arg: 3, scope: !312, file: !21, line: 87, type: !282)
!319 = !DILocalVariable(name: "z1Ptr", arg: 4, scope: !312, file: !21, line: 87, type: !282)
!320 = !DILocation(line: 92, column: 10, scope: !312)
!321 = !DILocalVariable(name: "aLow", scope: !312, file: !21, line: 89, type: !24)
!322 = !DILocation(line: 93, column: 13, scope: !312)
!323 = !DILocation(line: 93, column: 11, scope: !312)
!324 = !DILocalVariable(name: "aHigh", scope: !312, file: !21, line: 89, type: !24)
!325 = !DILocation(line: 94, column: 10, scope: !312)
!326 = !DILocalVariable(name: "bLow", scope: !312, file: !21, line: 89, type: !24)
!327 = !DILocation(line: 95, column: 13, scope: !312)
!328 = !DILocation(line: 95, column: 11, scope: !312)
!329 = !DILocalVariable(name: "bHigh", scope: !312, file: !21, line: 89, type: !24)
!330 = !DILocation(line: 96, column: 18, scope: !312)
!331 = !DILocation(line: 96, column: 26, scope: !312)
!332 = !DILocation(line: 96, column: 24, scope: !312)
!333 = !DILocalVariable(name: "z1", scope: !312, file: !21, line: 90, type: !6)
!334 = !DILocation(line: 97, column: 24, scope: !312)
!335 = !DILocation(line: 97, column: 32, scope: !312)
!336 = !DILocation(line: 97, column: 30, scope: !312)
!337 = !DILocalVariable(name: "zMiddleA", scope: !312, file: !21, line: 90, type: !6)
!338 = !DILocation(line: 98, column: 24, scope: !312)
!339 = !DILocation(line: 98, column: 33, scope: !312)
!340 = !DILocation(line: 98, column: 31, scope: !312)
!341 = !DILocalVariable(name: "zMiddleB", scope: !312, file: !21, line: 90, type: !6)
!342 = !DILocation(line: 99, column: 18, scope: !312)
!343 = !DILocation(line: 99, column: 27, scope: !312)
!344 = !DILocation(line: 99, column: 25, scope: !312)
!345 = !DILocalVariable(name: "z0", scope: !312, file: !21, line: 90, type: !6)
!346 = !DILocation(line: 100, column: 12, scope: !312)
!347 = !DILocation(line: 101, column: 30, scope: !312)
!348 = !DILocation(line: 101, column: 20, scope: !312)
!349 = !DILocation(line: 101, column: 43, scope: !312)
!350 = !DILocation(line: 101, column: 62, scope: !312)
!351 = !DILocation(line: 101, column: 50, scope: !312)
!352 = !DILocation(line: 101, column: 6, scope: !312)
!353 = !DILocation(line: 102, column: 12, scope: !312)
!354 = !DILocation(line: 103, column: 6, scope: !312)
!355 = !DILocation(line: 104, column: 13, scope: !312)
!356 = !DILocation(line: 104, column: 9, scope: !312)
!357 = !DILocation(line: 104, column: 6, scope: !312)
!358 = !DILocation(line: 105, column: 10, scope: !312)
!359 = !DILocation(line: 106, column: 10, scope: !312)
!360 = !DILocation(line: 108, column: 1, scope: !312)
!361 = distinct !DISubprogram(name: "float_raise", linkageName: "_Z11float_raisei", scope: !362, file: !362, line: 64, type: !363, scopeLine: 65, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!362 = !DIFile(filename: "dfmul/include/softfloat-specialize", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!363 = !DISubroutineType(types: !364)
!364 = !{null, !16}
!365 = !DILocalVariable(name: "flags", arg: 1, scope: !361, file: !362, line: 64, type: !16)
!366 = !DILocation(line: 0, scope: !361)
!367 = !DILocation(line: 66, column: 25, scope: !361)
!368 = !DILocation(line: 68, column: 1, scope: !361)
!369 = distinct !DISubprogram(name: "float64_is_nan", linkageName: "_Z14float64_is_nany", scope: !362, file: !362, line: 81, type: !370, scopeLine: 82, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!370 = !DISubroutineType(types: !371)
!371 = !{!372, !373}
!372 = !DIDerivedType(tag: DW_TAG_typedef, name: "flag", file: !7, line: 58, baseType: !17)
!373 = !DIDerivedType(tag: DW_TAG_typedef, name: "float64", file: !374, line: 54, baseType: !8)
!374 = !DIFile(filename: "dfmul/include/softfloat.h", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!375 = !DILocalVariable(name: "a", arg: 1, scope: !369, file: !362, line: 81, type: !373)
!376 = !DILocation(line: 0, scope: !369)
!377 = !DILocation(line: 84, column: 52, scope: !369)
!378 = !DILocation(line: 84, column: 38, scope: !369)
!379 = !DILocation(line: 84, column: 10, scope: !369)
!380 = !DILocation(line: 84, column: 3, scope: !369)
!381 = distinct !DISubprogram(name: "float64_is_signaling_nan", linkageName: "_Z24float64_is_signaling_nany", scope: !362, file: !362, line: 94, type: !370, scopeLine: 95, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!382 = !DILocalVariable(name: "a", arg: 1, scope: !381, file: !362, line: 94, type: !373)
!383 = !DILocation(line: 0, scope: !381)
!384 = !DILocation(line: 97, column: 15, scope: !381)
!385 = !DILocation(line: 97, column: 22, scope: !381)
!386 = !DILocation(line: 97, column: 31, scope: !381)
!387 = !DILocation(line: 97, column: 41, scope: !381)
!388 = !DILocation(line: 97, column: 47, scope: !381)
!389 = !DILocation(line: 97, column: 44, scope: !381)
!390 = !DILocation(line: 97, column: 10, scope: !381)
!391 = !DILocation(line: 97, column: 3, scope: !381)
!392 = distinct !DISubprogram(name: "extractFloat64Frac", scope: !3, file: !3, line: 89, type: !393, scopeLine: 89, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!393 = !DISubroutineType(types: !394)
!394 = !{!6, !373}
!395 = !DILocalVariable(name: "a", arg: 1, scope: !392, file: !3, line: 89, type: !373)
!396 = !DILocation(line: 0, scope: !392)
!397 = !DILocation(line: 90, column: 12, scope: !392)
!398 = !DILocation(line: 90, column: 3, scope: !392)
!399 = distinct !DISubprogram(name: "extractFloat64Exp", scope: !3, file: !3, line: 103, type: !400, scopeLine: 103, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!400 = !DISubroutineType(types: !401)
!401 = !{!281, !373}
!402 = !DILocalVariable(name: "a", arg: 1, scope: !399, file: !3, line: 103, type: !373)
!403 = !DILocation(line: 0, scope: !399)
!404 = !DILocation(line: 104, column: 13, scope: !399)
!405 = !DILocation(line: 104, column: 20, scope: !399)
!406 = !DILocation(line: 104, column: 10, scope: !399)
!407 = !DILocation(line: 104, column: 3, scope: !399)
!408 = distinct !DISubprogram(name: "extractFloat64Sign", scope: !3, file: !3, line: 117, type: !370, scopeLine: 117, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!409 = !DILocalVariable(name: "a", arg: 1, scope: !408, file: !3, line: 117, type: !373)
!410 = !DILocation(line: 0, scope: !408)
!411 = !DILocation(line: 118, column: 12, scope: !408)
!412 = !DILocation(line: 118, column: 10, scope: !408)
!413 = !DILocation(line: 118, column: 3, scope: !408)
!414 = distinct !DISubprogram(name: "normalizeFloat64Subnormal", scope: !3, file: !3, line: 133, type: !415, scopeLine: 133, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!415 = !DISubroutineType(types: !416)
!416 = !{null, !6, !417, !282}
!417 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !281, size: 64)
!418 = !DILocalVariable(name: "aSig", arg: 1, scope: !414, file: !3, line: 133, type: !6)
!419 = !DILocation(line: 0, scope: !414)
!420 = !DILocalVariable(name: "zExpPtr", arg: 2, scope: !414, file: !3, line: 133, type: !417)
!421 = !DILocalVariable(name: "zSigPtr", arg: 3, scope: !414, file: !3, line: 133, type: !282)
!422 = !DILocation(line: 136, column: 16, scope: !414)
!423 = !DILocation(line: 136, column: 43, scope: !414)
!424 = !DILocalVariable(name: "shiftCount", scope: !414, file: !3, line: 134, type: !16)
!425 = !DILocation(line: 137, column: 19, scope: !414)
!426 = !DILocation(line: 137, column: 12, scope: !414)
!427 = !DILocation(line: 138, column: 16, scope: !414)
!428 = !DILocation(line: 138, column: 12, scope: !414)
!429 = !DILocation(line: 140, column: 1, scope: !414)
!430 = distinct !DISubprogram(name: "countLeadingZeros64", linkageName: "_ZL19countLeadingZeros64y", scope: !21, file: !21, line: 160, type: !431, scopeLine: 161, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!431 = !DISubroutineType(types: !432)
!432 = !{!16, !6}
!433 = !DILocalVariable(name: "a", arg: 1, scope: !430, file: !21, line: 160, type: !6)
!434 = !DILocation(line: 0, scope: !430)
!435 = !DILocalVariable(name: "shiftCount", scope: !430, file: !21, line: 162, type: !16)
!436 = !DILocation(line: 165, column: 9, scope: !437)
!437 = distinct !DILexicalBlock(scope: !430, file: !21, line: 165, column: 7)
!438 = !DILocation(line: 165, column: 7, scope: !430)
!439 = !DILocation(line: 167, column: 18, scope: !440)
!440 = distinct !DILexicalBlock(scope: !437, file: !21, line: 166, column: 5)
!441 = !DILocation(line: 168, column: 5, scope: !440)
!442 = !DILocation(line: 171, column: 9, scope: !443)
!443 = distinct !DILexicalBlock(scope: !437, file: !21, line: 170, column: 5)
!444 = !DILocation(line: 173, column: 38, scope: !430)
!445 = !DILocation(line: 173, column: 17, scope: !430)
!446 = !DILocation(line: 173, column: 14, scope: !430)
!447 = !DILocation(line: 174, column: 3, scope: !430)
!448 = distinct !DISubprogram(name: "packFloat64", scope: !3, file: !3, line: 159, type: !449, scopeLine: 159, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!449 = !DISubroutineType(types: !450)
!450 = !{!373, !372, !281, !6}
!451 = !DILocalVariable(name: "zSign", arg: 1, scope: !448, file: !3, line: 159, type: !372)
!452 = !DILocation(line: 0, scope: !448)
!453 = !DILocalVariable(name: "zExp", arg: 2, scope: !448, file: !3, line: 159, type: !281)
!454 = !DILocalVariable(name: "zSig", arg: 3, scope: !448, file: !3, line: 159, type: !6)
!455 = !DILocation(line: 160, column: 21, scope: !448)
!456 = !DILocation(line: 160, column: 28, scope: !448)
!457 = !DILocation(line: 160, column: 48, scope: !448)
!458 = !DILocation(line: 160, column: 54, scope: !448)
!459 = !DILocation(line: 160, column: 35, scope: !448)
!460 = !DILocation(line: 160, column: 61, scope: !448)
!461 = !DILocation(line: 160, column: 3, scope: !448)
!462 = distinct !DISubprogram(name: "roundAndPackFloat64", scope: !3, file: !3, line: 190, type: !449, scopeLine: 190, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!463 = !DILocalVariable(name: "zSign", arg: 1, scope: !462, file: !3, line: 190, type: !372)
!464 = !DILocation(line: 0, scope: !462)
!465 = !DILocalVariable(name: "zExp", arg: 2, scope: !462, file: !3, line: 190, type: !281)
!466 = !DILocalVariable(name: "zSig", arg: 3, scope: !462, file: !3, line: 190, type: !6)
!467 = !DILocation(line: 190, column: 61, scope: !462)
!468 = !DILocation(line: 195, column: 18, scope: !462)
!469 = !DILocalVariable(name: "roundingMode", scope: !462, file: !3, line: 191, type: !16)
!470 = !DILocation(line: 196, column: 36, scope: !462)
!471 = !DILocation(line: 196, column: 22, scope: !462)
!472 = !DILocalVariable(name: "roundNearestEven", scope: !462, file: !3, line: 192, type: !372)
!473 = !DILocalVariable(name: "roundIncrement", scope: !462, file: !3, line: 193, type: !281)
!474 = !DILocation(line: 198, column: 8, scope: !475)
!475 = distinct !DILexicalBlock(scope: !462, file: !3, line: 198, column: 7)
!476 = !DILocation(line: 198, column: 7, scope: !462)
!477 = !DILocation(line: 200, column: 24, scope: !478)
!478 = distinct !DILexicalBlock(scope: !479, file: !3, line: 200, column: 11)
!479 = distinct !DILexicalBlock(scope: !475, file: !3, line: 199, column: 5)
!480 = !DILocation(line: 200, column: 11, scope: !479)
!481 = !DILocation(line: 203, column: 2, scope: !482)
!482 = distinct !DILexicalBlock(scope: !478, file: !3, line: 201, column: 2)
!483 = !DILocation(line: 207, column: 8, scope: !484)
!484 = distinct !DILexicalBlock(scope: !485, file: !3, line: 207, column: 8)
!485 = distinct !DILexicalBlock(scope: !478, file: !3, line: 205, column: 2)
!486 = !DILocation(line: 207, column: 8, scope: !485)
!487 = !DILocation(line: 209, column: 25, scope: !488)
!488 = distinct !DILexicalBlock(scope: !489, file: !3, line: 209, column: 12)
!489 = distinct !DILexicalBlock(scope: !484, file: !3, line: 208, column: 6)
!490 = !DILocation(line: 209, column: 12, scope: !489)
!491 = !DILocation(line: 210, column: 3, scope: !488)
!492 = !DILocation(line: 0, scope: !485)
!493 = !DILocation(line: 211, column: 6, scope: !489)
!494 = !DILocation(line: 214, column: 25, scope: !495)
!495 = distinct !DILexicalBlock(scope: !496, file: !3, line: 214, column: 12)
!496 = distinct !DILexicalBlock(scope: !484, file: !3, line: 213, column: 6)
!497 = !DILocation(line: 214, column: 12, scope: !496)
!498 = !DILocation(line: 215, column: 3, scope: !495)
!499 = !DILocation(line: 0, scope: !484)
!500 = !DILocation(line: 0, scope: !478)
!501 = !DILocation(line: 218, column: 5, scope: !479)
!502 = !DILocation(line: 219, column: 15, scope: !462)
!503 = !DILocation(line: 219, column: 20, scope: !462)
!504 = !DILocalVariable(name: "roundBits", scope: !462, file: !3, line: 193, type: !281)
!505 = !DILocation(line: 220, column: 25, scope: !506)
!506 = distinct !DILexicalBlock(scope: !462, file: !3, line: 220, column: 7)
!507 = !DILocation(line: 220, column: 16, scope: !506)
!508 = !DILocation(line: 220, column: 13, scope: !506)
!509 = !DILocation(line: 220, column: 7, scope: !462)
!510 = !DILocation(line: 222, column: 18, scope: !511)
!511 = distinct !DILexicalBlock(scope: !512, file: !3, line: 222, column: 11)
!512 = distinct !DILexicalBlock(scope: !506, file: !3, line: 221, column: 5)
!513 = !DILocation(line: 223, column: 4, scope: !511)
!514 = !DILocation(line: 223, column: 14, scope: !511)
!515 = !DILocation(line: 223, column: 24, scope: !511)
!516 = !DILocation(line: 223, column: 39, scope: !511)
!517 = !DILocation(line: 223, column: 46, scope: !511)
!518 = !DILocation(line: 223, column: 44, scope: !511)
!519 = !DILocation(line: 223, column: 62, scope: !511)
!520 = !DILocation(line: 222, column: 11, scope: !512)
!521 = !DILocation(line: 225, column: 4, scope: !522)
!522 = distinct !DILexicalBlock(scope: !511, file: !3, line: 224, column: 2)
!523 = !DILocation(line: 0, scope: !448, inlinedAt: !524)
!524 = distinct !DILocation(line: 226, column: 11, scope: !522)
!525 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !524)
!526 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !524)
!527 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !524)
!528 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !524)
!529 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !524)
!530 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !524)
!531 = !DILocation(line: 226, column: 59, scope: !522)
!532 = !DILocation(line: 226, column: 43, scope: !522)
!533 = !DILocation(line: 226, column: 41, scope: !522)
!534 = !DILocation(line: 226, column: 4, scope: !522)
!535 = !DILocation(line: 228, column: 16, scope: !536)
!536 = distinct !DILexicalBlock(scope: !512, file: !3, line: 228, column: 11)
!537 = !DILocation(line: 228, column: 11, scope: !512)
!538 = !DILocalVariable(name: "isTiny", scope: !462, file: !3, line: 192, type: !372)
!539 = !DILocation(line: 233, column: 25, scope: !540)
!540 = distinct !DILexicalBlock(scope: !536, file: !3, line: 229, column: 2)
!541 = !DILocation(line: 233, column: 31, scope: !540)
!542 = !DILocation(line: 233, column: 4, scope: !540)
!543 = !DILocation(line: 235, column: 16, scope: !540)
!544 = !DILocation(line: 235, column: 21, scope: !540)
!545 = !DILocation(line: 236, column: 8, scope: !546)
!546 = distinct !DILexicalBlock(scope: !540, file: !3, line: 236, column: 8)
!547 = !DILocation(line: 236, column: 15, scope: !546)
!548 = !DILocation(line: 236, column: 18, scope: !546)
!549 = !DILocation(line: 236, column: 8, scope: !540)
!550 = !DILocation(line: 237, column: 6, scope: !546)
!551 = !DILocation(line: 238, column: 2, scope: !540)
!552 = !DILocation(line: 239, column: 5, scope: !512)
!553 = !DILocation(line: 240, column: 7, scope: !554)
!554 = distinct !DILexicalBlock(scope: !462, file: !3, line: 240, column: 7)
!555 = !DILocation(line: 240, column: 7, scope: !462)
!556 = !DILocation(line: 241, column: 27, scope: !554)
!557 = !DILocation(line: 241, column: 5, scope: !554)
!558 = !DILocation(line: 242, column: 11, scope: !462)
!559 = !DILocation(line: 242, column: 18, scope: !462)
!560 = !DILocation(line: 242, column: 16, scope: !462)
!561 = !DILocation(line: 242, column: 34, scope: !462)
!562 = !DILocation(line: 242, column: 8, scope: !462)
!563 = !DILocation(line: 243, column: 25, scope: !462)
!564 = !DILocation(line: 243, column: 34, scope: !462)
!565 = !DILocation(line: 243, column: 13, scope: !462)
!566 = !DILocation(line: 243, column: 40, scope: !462)
!567 = !DILocation(line: 243, column: 11, scope: !462)
!568 = !DILocation(line: 243, column: 8, scope: !462)
!569 = !DILocation(line: 244, column: 7, scope: !570)
!570 = distinct !DILexicalBlock(scope: !462, file: !3, line: 244, column: 7)
!571 = !DILocation(line: 244, column: 12, scope: !570)
!572 = !DILocation(line: 244, column: 7, scope: !462)
!573 = !DILocation(line: 245, column: 5, scope: !570)
!574 = !DILocation(line: 246, column: 36, scope: !462)
!575 = !DILocation(line: 0, scope: !448, inlinedAt: !576)
!576 = distinct !DILocation(line: 246, column: 10, scope: !462)
!577 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !576)
!578 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !576)
!579 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !576)
!580 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !576)
!581 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !576)
!582 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !576)
!583 = !DILocation(line: 246, column: 3, scope: !462)
!584 = !DILocation(line: 248, column: 1, scope: !462)
!585 = distinct !DISubprogram(name: "float64_mul", scope: !3, file: !3, line: 272, type: !586, scopeLine: 272, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!586 = !DISubroutineType(types: !587)
!587 = !{!373, !588, !589}
!588 = !DIDerivedType(tag: DW_TAG_typedef, name: "bmx055xAcceleration", file: !3, line: 262, baseType: !373)
!589 = !DIDerivedType(tag: DW_TAG_typedef, name: "bmx055yAcceleration", file: !3, line: 263, baseType: !373)
!590 = !DILocalVariable(name: "a", arg: 1, scope: !585, file: !3, line: 272, type: !588)
!591 = !DILocation(line: 0, scope: !585)
!592 = !DILocalVariable(name: "b", arg: 2, scope: !585, file: !3, line: 272, type: !589)
!593 = !DILocalVariable(name: "aExp", scope: !585, file: !3, line: 284, type: !281)
!594 = !DILocation(line: 284, column: 9, scope: !585)
!595 = !DILocalVariable(name: "bExp", scope: !585, file: !3, line: 284, type: !281)
!596 = !DILocation(line: 284, column: 15, scope: !585)
!597 = !DILocalVariable(name: "aSig", scope: !585, file: !3, line: 285, type: !6)
!598 = !DILocation(line: 285, column: 10, scope: !585)
!599 = !DILocalVariable(name: "bSig", scope: !585, file: !3, line: 285, type: !6)
!600 = !DILocation(line: 285, column: 16, scope: !585)
!601 = !DILocalVariable(name: "zSig0", scope: !585, file: !3, line: 285, type: !6)
!602 = !DILocation(line: 285, column: 22, scope: !585)
!603 = !DILocalVariable(name: "zSig1", scope: !585, file: !3, line: 285, type: !6)
!604 = !DILocation(line: 285, column: 29, scope: !585)
!605 = !DILocation(line: 0, scope: !392, inlinedAt: !606)
!606 = distinct !DILocation(line: 287, column: 10, scope: !585)
!607 = !DILocation(line: 90, column: 12, scope: !392, inlinedAt: !606)
!608 = !DILocation(line: 287, column: 8, scope: !585)
!609 = !DILocation(line: 0, scope: !399, inlinedAt: !610)
!610 = distinct !DILocation(line: 288, column: 10, scope: !585)
!611 = !DILocation(line: 104, column: 13, scope: !399, inlinedAt: !610)
!612 = !DILocation(line: 104, column: 20, scope: !399, inlinedAt: !610)
!613 = !DILocation(line: 104, column: 10, scope: !399, inlinedAt: !610)
!614 = !DILocation(line: 288, column: 8, scope: !585)
!615 = !DILocation(line: 0, scope: !408, inlinedAt: !616)
!616 = distinct !DILocation(line: 289, column: 11, scope: !585)
!617 = !DILocation(line: 118, column: 12, scope: !408, inlinedAt: !616)
!618 = !DILocation(line: 118, column: 10, scope: !408, inlinedAt: !616)
!619 = !DILocalVariable(name: "aSign", scope: !585, file: !3, line: 283, type: !372)
!620 = !DILocation(line: 0, scope: !392, inlinedAt: !621)
!621 = distinct !DILocation(line: 290, column: 10, scope: !585)
!622 = !DILocation(line: 90, column: 12, scope: !392, inlinedAt: !621)
!623 = !DILocation(line: 290, column: 8, scope: !585)
!624 = !DILocation(line: 0, scope: !399, inlinedAt: !625)
!625 = distinct !DILocation(line: 291, column: 10, scope: !585)
!626 = !DILocation(line: 104, column: 13, scope: !399, inlinedAt: !625)
!627 = !DILocation(line: 104, column: 20, scope: !399, inlinedAt: !625)
!628 = !DILocation(line: 104, column: 10, scope: !399, inlinedAt: !625)
!629 = !DILocation(line: 291, column: 8, scope: !585)
!630 = !DILocation(line: 0, scope: !408, inlinedAt: !631)
!631 = distinct !DILocation(line: 292, column: 11, scope: !585)
!632 = !DILocation(line: 118, column: 12, scope: !408, inlinedAt: !631)
!633 = !DILocation(line: 118, column: 10, scope: !408, inlinedAt: !631)
!634 = !DILocalVariable(name: "bSign", scope: !585, file: !3, line: 283, type: !372)
!635 = !DILocation(line: 293, column: 17, scope: !585)
!636 = !DILocalVariable(name: "zSign", scope: !585, file: !3, line: 283, type: !372)
!637 = !DILocation(line: 294, column: 7, scope: !638)
!638 = distinct !DILexicalBlock(scope: !585, file: !3, line: 294, column: 7)
!639 = !DILocation(line: 294, column: 12, scope: !638)
!640 = !DILocation(line: 294, column: 7, scope: !585)
!641 = !DILocation(line: 296, column: 11, scope: !642)
!642 = distinct !DILexicalBlock(scope: !643, file: !3, line: 296, column: 11)
!643 = distinct !DILexicalBlock(scope: !638, file: !3, line: 295, column: 5)
!644 = !DILocation(line: 296, column: 16, scope: !642)
!645 = !DILocation(line: 296, column: 21, scope: !642)
!646 = !DILocation(line: 296, column: 26, scope: !642)
!647 = !DILocation(line: 296, column: 36, scope: !642)
!648 = !DILocation(line: 296, column: 39, scope: !642)
!649 = !DILocation(line: 296, column: 11, scope: !643)
!650 = !DILocation(line: 297, column: 9, scope: !642)
!651 = !DILocation(line: 297, column: 2, scope: !642)
!652 = !DILocation(line: 298, column: 12, scope: !653)
!653 = distinct !DILexicalBlock(scope: !643, file: !3, line: 298, column: 11)
!654 = !DILocation(line: 298, column: 19, scope: !653)
!655 = !DILocation(line: 298, column: 17, scope: !653)
!656 = !DILocation(line: 298, column: 25, scope: !653)
!657 = !DILocation(line: 298, column: 11, scope: !643)
!658 = !DILocation(line: 300, column: 4, scope: !659)
!659 = distinct !DILexicalBlock(scope: !653, file: !3, line: 299, column: 2)
!660 = !DILocation(line: 301, column: 4, scope: !659)
!661 = !DILocation(line: 0, scope: !448, inlinedAt: !662)
!662 = distinct !DILocation(line: 303, column: 14, scope: !643)
!663 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !662)
!664 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !662)
!665 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !662)
!666 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !662)
!667 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !662)
!668 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !662)
!669 = !DILocation(line: 303, column: 7, scope: !643)
!670 = !DILocation(line: 305, column: 7, scope: !671)
!671 = distinct !DILexicalBlock(scope: !585, file: !3, line: 305, column: 7)
!672 = !DILocation(line: 305, column: 12, scope: !671)
!673 = !DILocation(line: 305, column: 7, scope: !585)
!674 = !DILocation(line: 307, column: 11, scope: !675)
!675 = distinct !DILexicalBlock(scope: !676, file: !3, line: 307, column: 11)
!676 = distinct !DILexicalBlock(scope: !671, file: !3, line: 306, column: 5)
!677 = !DILocation(line: 307, column: 11, scope: !676)
!678 = !DILocation(line: 308, column: 9, scope: !675)
!679 = !DILocation(line: 308, column: 2, scope: !675)
!680 = !DILocation(line: 309, column: 12, scope: !681)
!681 = distinct !DILexicalBlock(scope: !676, file: !3, line: 309, column: 11)
!682 = !DILocation(line: 309, column: 19, scope: !681)
!683 = !DILocation(line: 309, column: 17, scope: !681)
!684 = !DILocation(line: 309, column: 25, scope: !681)
!685 = !DILocation(line: 309, column: 11, scope: !676)
!686 = !DILocation(line: 311, column: 4, scope: !687)
!687 = distinct !DILexicalBlock(scope: !681, file: !3, line: 310, column: 2)
!688 = !DILocation(line: 312, column: 4, scope: !687)
!689 = !DILocation(line: 0, scope: !448, inlinedAt: !690)
!690 = distinct !DILocation(line: 314, column: 14, scope: !676)
!691 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !690)
!692 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !690)
!693 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !690)
!694 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !690)
!695 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !690)
!696 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !690)
!697 = !DILocation(line: 314, column: 7, scope: !676)
!698 = !DILocation(line: 316, column: 7, scope: !699)
!699 = distinct !DILexicalBlock(scope: !585, file: !3, line: 316, column: 7)
!700 = !DILocation(line: 316, column: 12, scope: !699)
!701 = !DILocation(line: 316, column: 7, scope: !585)
!702 = !DILocation(line: 318, column: 11, scope: !703)
!703 = distinct !DILexicalBlock(scope: !704, file: !3, line: 318, column: 11)
!704 = distinct !DILexicalBlock(scope: !699, file: !3, line: 317, column: 5)
!705 = !DILocation(line: 318, column: 16, scope: !703)
!706 = !DILocation(line: 318, column: 11, scope: !704)
!707 = !DILocation(line: 0, scope: !448, inlinedAt: !708)
!708 = distinct !DILocation(line: 319, column: 9, scope: !703)
!709 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !708)
!710 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !708)
!711 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !708)
!712 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !708)
!713 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !708)
!714 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !708)
!715 = !DILocation(line: 319, column: 2, scope: !703)
!716 = !DILocation(line: 320, column: 34, scope: !704)
!717 = !DILocation(line: 320, column: 7, scope: !704)
!718 = !DILocation(line: 321, column: 5, scope: !704)
!719 = !DILocation(line: 322, column: 7, scope: !720)
!720 = distinct !DILexicalBlock(scope: !585, file: !3, line: 322, column: 7)
!721 = !DILocation(line: 322, column: 12, scope: !720)
!722 = !DILocation(line: 322, column: 7, scope: !585)
!723 = !DILocation(line: 324, column: 11, scope: !724)
!724 = distinct !DILexicalBlock(scope: !725, file: !3, line: 324, column: 11)
!725 = distinct !DILexicalBlock(scope: !720, file: !3, line: 323, column: 5)
!726 = !DILocation(line: 324, column: 16, scope: !724)
!727 = !DILocation(line: 324, column: 11, scope: !725)
!728 = !DILocation(line: 0, scope: !448, inlinedAt: !729)
!729 = distinct !DILocation(line: 325, column: 9, scope: !724)
!730 = !DILocation(line: 160, column: 21, scope: !448, inlinedAt: !729)
!731 = !DILocation(line: 160, column: 28, scope: !448, inlinedAt: !729)
!732 = !DILocation(line: 160, column: 48, scope: !448, inlinedAt: !729)
!733 = !DILocation(line: 160, column: 54, scope: !448, inlinedAt: !729)
!734 = !DILocation(line: 160, column: 35, scope: !448, inlinedAt: !729)
!735 = !DILocation(line: 160, column: 61, scope: !448, inlinedAt: !729)
!736 = !DILocation(line: 325, column: 2, scope: !724)
!737 = !DILocation(line: 326, column: 34, scope: !725)
!738 = !DILocation(line: 326, column: 7, scope: !725)
!739 = !DILocation(line: 327, column: 5, scope: !725)
!740 = !DILocation(line: 328, column: 10, scope: !585)
!741 = !DILocation(line: 328, column: 17, scope: !585)
!742 = !DILocation(line: 328, column: 15, scope: !585)
!743 = !DILocation(line: 328, column: 22, scope: !585)
!744 = !DILocalVariable(name: "zExp", scope: !585, file: !3, line: 284, type: !281)
!745 = !DILocation(line: 329, column: 11, scope: !585)
!746 = !DILocation(line: 329, column: 16, scope: !585)
!747 = !DILocation(line: 329, column: 46, scope: !585)
!748 = !DILocation(line: 329, column: 8, scope: !585)
!749 = !DILocation(line: 330, column: 11, scope: !585)
!750 = !DILocation(line: 330, column: 16, scope: !585)
!751 = !DILocation(line: 330, column: 46, scope: !585)
!752 = !DILocation(line: 330, column: 8, scope: !585)
!753 = !DILocation(line: 331, column: 15, scope: !585)
!754 = !DILocation(line: 331, column: 21, scope: !585)
!755 = !DILocation(line: 331, column: 3, scope: !585)
!756 = !DILocation(line: 332, column: 13, scope: !585)
!757 = !DILocation(line: 332, column: 19, scope: !585)
!758 = !DILocation(line: 332, column: 12, scope: !585)
!759 = !DILocation(line: 332, column: 9, scope: !585)
!760 = !DILocation(line: 333, column: 23, scope: !761)
!761 = distinct !DILexicalBlock(scope: !585, file: !3, line: 333, column: 7)
!762 = !DILocation(line: 333, column: 29, scope: !761)
!763 = !DILocation(line: 333, column: 9, scope: !761)
!764 = !DILocation(line: 333, column: 7, scope: !585)
!765 = !DILocation(line: 335, column: 13, scope: !766)
!766 = distinct !DILexicalBlock(scope: !761, file: !3, line: 334, column: 5)
!767 = !DILocation(line: 336, column: 7, scope: !766)
!768 = !DILocation(line: 337, column: 5, scope: !766)
!769 = !DILocation(line: 338, column: 44, scope: !585)
!770 = !DILocation(line: 338, column: 10, scope: !585)
!771 = !DILocation(line: 338, column: 3, scope: !585)
!772 = !DILocation(line: 340, column: 1, scope: !585)
!773 = distinct !DISubprogram(name: "propagateFloat64NaN", linkageName: "_ZL19propagateFloat64NaNyy", scope: !362, file: !362, line: 108, type: !774, scopeLine: 109, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!774 = !DISubroutineType(types: !775)
!775 = !{!373, !373, !373}
!776 = !DILocalVariable(name: "a", arg: 1, scope: !773, file: !362, line: 108, type: !373)
!777 = !DILocation(line: 0, scope: !773)
!778 = !DILocalVariable(name: "b", arg: 2, scope: !773, file: !362, line: 108, type: !373)
!779 = !DILocation(line: 112, column: 12, scope: !773)
!780 = !DILocalVariable(name: "aIsNaN", scope: !773, file: !362, line: 110, type: !372)
!781 = !DILocation(line: 113, column: 21, scope: !773)
!782 = !DILocalVariable(name: "aIsSignalingNaN", scope: !773, file: !362, line: 110, type: !372)
!783 = !DILocation(line: 114, column: 12, scope: !773)
!784 = !DILocalVariable(name: "bIsNaN", scope: !773, file: !362, line: 110, type: !372)
!785 = !DILocation(line: 115, column: 21, scope: !773)
!786 = !DILocalVariable(name: "bIsSignalingNaN", scope: !773, file: !362, line: 110, type: !372)
!787 = !DILocation(line: 116, column: 5, scope: !773)
!788 = !DILocation(line: 117, column: 5, scope: !773)
!789 = !DILocation(line: 118, column: 23, scope: !790)
!790 = distinct !DILexicalBlock(scope: !773, file: !362, line: 118, column: 7)
!791 = !DILocation(line: 118, column: 7, scope: !790)
!792 = !DILocation(line: 118, column: 7, scope: !773)
!793 = !DILocation(line: 119, column: 5, scope: !790)
!794 = !DILocation(line: 120, column: 10, scope: !773)
!795 = !DILocation(line: 120, column: 32, scope: !773)
!796 = !DILocation(line: 120, column: 54, scope: !773)
!797 = !DILocation(line: 120, column: 3, scope: !773)
!798 = !DILocalVariable(name: "a", arg: 1, scope: !20, file: !21, line: 116, type: !24)
!799 = !DILocation(line: 0, scope: !20)
!800 = !DILocalVariable(name: "shiftCount", scope: !20, file: !21, line: 136, type: !16)
!801 = !DILocation(line: 139, column: 9, scope: !802)
!802 = distinct !DILexicalBlock(scope: !20, file: !21, line: 139, column: 7)
!803 = !DILocation(line: 139, column: 7, scope: !20)
!804 = !DILocation(line: 141, column: 18, scope: !805)
!805 = distinct !DILexicalBlock(scope: !802, file: !21, line: 140, column: 5)
!806 = !DILocation(line: 142, column: 9, scope: !805)
!807 = !DILocation(line: 143, column: 5, scope: !805)
!808 = !DILocation(line: 144, column: 9, scope: !809)
!809 = distinct !DILexicalBlock(scope: !20, file: !21, line: 144, column: 7)
!810 = !DILocation(line: 144, column: 7, scope: !20)
!811 = !DILocation(line: 146, column: 18, scope: !812)
!812 = distinct !DILexicalBlock(scope: !809, file: !21, line: 145, column: 5)
!813 = !DILocation(line: 147, column: 9, scope: !812)
!814 = !DILocation(line: 148, column: 5, scope: !812)
!815 = !DILocation(line: 149, column: 41, scope: !20)
!816 = !DILocation(line: 149, column: 17, scope: !20)
!817 = !DILocation(line: 149, column: 14, scope: !20)
!818 = !DILocation(line: 150, column: 3, scope: !20)
