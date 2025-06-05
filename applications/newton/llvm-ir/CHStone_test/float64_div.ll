; ModuleID = 'float64_div.ll'
source_filename = "dfdiv/float64_div.cpp"
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
define dso_local void @_Z6add128yyyyPyS_(i64 %0, i64 %1, i64 %2, i64 %3, i64* %4, i64* %5) #0 !dbg !312 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !315, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64 %1, metadata !317, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64 %2, metadata !318, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64 %3, metadata !319, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64* %4, metadata !320, metadata !DIExpression()), !dbg !316
  call void @llvm.dbg.value(metadata i64* %5, metadata !321, metadata !DIExpression()), !dbg !316
  %7 = add i64 %1, %3, !dbg !322
  call void @llvm.dbg.value(metadata i64 %7, metadata !323, metadata !DIExpression()), !dbg !316
  store i64 %7, i64* %5, align 8, !dbg !324
  %8 = add i64 %0, %2, !dbg !325
  %9 = icmp ult i64 %7, %1, !dbg !326
  %10 = zext i1 %9 to i64, !dbg !327
  %11 = add i64 %8, %10, !dbg !328
  store i64 %11, i64* %4, align 8, !dbg !329
  ret void, !dbg !330
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z6sub128yyyyPyS_(i64 %0, i64 %1, i64 %2, i64 %3, i64* %4, i64* %5) #0 !dbg !331 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !332, metadata !DIExpression()), !dbg !333
  call void @llvm.dbg.value(metadata i64 %1, metadata !334, metadata !DIExpression()), !dbg !333
  call void @llvm.dbg.value(metadata i64 %2, metadata !335, metadata !DIExpression()), !dbg !333
  call void @llvm.dbg.value(metadata i64 %3, metadata !336, metadata !DIExpression()), !dbg !333
  call void @llvm.dbg.value(metadata i64* %4, metadata !337, metadata !DIExpression()), !dbg !333
  call void @llvm.dbg.value(metadata i64* %5, metadata !338, metadata !DIExpression()), !dbg !333
  %7 = sub i64 %1, %3, !dbg !339
  store i64 %7, i64* %5, align 8, !dbg !340
  %8 = sub i64 %0, %2, !dbg !341
  %9 = icmp ult i64 %1, %3, !dbg !342
  %10 = zext i1 %9 to i64, !dbg !343
  %11 = sub i64 %8, %10, !dbg !344
  store i64 %11, i64* %4, align 8, !dbg !345
  ret void, !dbg !346
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z10mul64To128yyPyS_(i64 %0, i64 %1, i64* %2, i64* %3) #0 !dbg !347 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !350, metadata !DIExpression()), !dbg !351
  call void @llvm.dbg.value(metadata i64 %1, metadata !352, metadata !DIExpression()), !dbg !351
  call void @llvm.dbg.value(metadata i64* %2, metadata !353, metadata !DIExpression()), !dbg !351
  call void @llvm.dbg.value(metadata i64* %3, metadata !354, metadata !DIExpression()), !dbg !351
  %5 = trunc i64 %0 to i32, !dbg !355
  call void @llvm.dbg.value(metadata i32 %5, metadata !356, metadata !DIExpression()), !dbg !351
  %6 = lshr i64 %0, 32, !dbg !357
  %7 = trunc i64 %6 to i32, !dbg !358
  call void @llvm.dbg.value(metadata i32 %7, metadata !359, metadata !DIExpression()), !dbg !351
  %8 = trunc i64 %1 to i32, !dbg !360
  call void @llvm.dbg.value(metadata i32 %8, metadata !361, metadata !DIExpression()), !dbg !351
  %9 = lshr i64 %1, 32, !dbg !362
  %10 = trunc i64 %9 to i32, !dbg !363
  call void @llvm.dbg.value(metadata i32 %10, metadata !364, metadata !DIExpression()), !dbg !351
  %11 = zext i32 %5 to i64, !dbg !365
  %12 = zext i32 %8 to i64, !dbg !366
  %13 = mul i64 %11, %12, !dbg !367
  call void @llvm.dbg.value(metadata i64 %13, metadata !368, metadata !DIExpression()), !dbg !351
  %14 = zext i32 %5 to i64, !dbg !369
  %15 = zext i32 %10 to i64, !dbg !370
  %16 = mul i64 %14, %15, !dbg !371
  call void @llvm.dbg.value(metadata i64 %16, metadata !372, metadata !DIExpression()), !dbg !351
  %17 = zext i32 %7 to i64, !dbg !373
  %18 = zext i32 %8 to i64, !dbg !374
  %19 = mul i64 %17, %18, !dbg !375
  call void @llvm.dbg.value(metadata i64 %19, metadata !376, metadata !DIExpression()), !dbg !351
  %20 = zext i32 %7 to i64, !dbg !377
  %21 = zext i32 %10 to i64, !dbg !378
  %22 = mul i64 %20, %21, !dbg !379
  call void @llvm.dbg.value(metadata i64 %22, metadata !380, metadata !DIExpression()), !dbg !351
  %23 = add i64 %16, %19, !dbg !381
  call void @llvm.dbg.value(metadata i64 %23, metadata !372, metadata !DIExpression()), !dbg !351
  %24 = icmp ult i64 %23, %19, !dbg !382
  %25 = zext i1 %24 to i64, !dbg !383
  %26 = shl i64 %25, 32, !dbg !384
  %27 = lshr i64 %23, 32, !dbg !385
  %28 = add i64 %26, %27, !dbg !386
  %29 = add i64 %22, %28, !dbg !387
  call void @llvm.dbg.value(metadata i64 %29, metadata !380, metadata !DIExpression()), !dbg !351
  %30 = shl i64 %23, 32, !dbg !388
  call void @llvm.dbg.value(metadata i64 %30, metadata !372, metadata !DIExpression()), !dbg !351
  %31 = add i64 %13, %30, !dbg !389
  call void @llvm.dbg.value(metadata i64 %31, metadata !368, metadata !DIExpression()), !dbg !351
  %32 = icmp ult i64 %31, %30, !dbg !390
  %33 = zext i1 %32 to i64, !dbg !391
  %34 = add i64 %29, %33, !dbg !392
  call void @llvm.dbg.value(metadata i64 %34, metadata !380, metadata !DIExpression()), !dbg !351
  store i64 %31, i64* %3, align 8, !dbg !393
  store i64 %34, i64* %2, align 8, !dbg !394
  ret void, !dbg !395
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z11float_raisei(i32 %0) #0 !dbg !396 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !400, metadata !DIExpression()), !dbg !401
  %2 = load i32, i32* @float_exception_flags, align 4, !dbg !402
  %3 = or i32 %2, %0, !dbg !402
  store i32 %3, i32* @float_exception_flags, align 4, !dbg !402
  ret void, !dbg !403
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local i32 @_Z14float64_is_nany(i64 %0) #0 !dbg !404 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !410, metadata !DIExpression()), !dbg !411
  %2 = shl i64 %0, 1, !dbg !412
  %3 = icmp ult i64 -9007199254740992, %2, !dbg !413
  %4 = zext i1 %3 to i32, !dbg !414
  ret i32 %4, !dbg !415
}

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local i32 @_Z24float64_is_signaling_nany(i64 %0) #0 !dbg !416 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !417, metadata !DIExpression()), !dbg !418
  %2 = lshr i64 %0, 51, !dbg !419
  %3 = and i64 %2, 4095, !dbg !420
  %4 = icmp eq i64 %3, 4094, !dbg !421
  br i1 %4, label %5, label %8, !dbg !422

5:                                                ; preds = %1
  %6 = and i64 %0, 2251799813685247, !dbg !423
  %7 = icmp ne i64 %6, 0, !dbg !424
  br label %8

8:                                                ; preds = %5, %1
  %9 = phi i1 [ false, %1 ], [ %7, %5 ], !dbg !418
  %10 = zext i1 %9 to i32, !dbg !425
  ret i32 %10, !dbg !426
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i64 @extractFloat64Frac(i64 %0) #2 !dbg !427 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !430, metadata !DIExpression()), !dbg !431
  %2 = and i64 %0, 4503599627370495, !dbg !432
  ret i64 %2, !dbg !433
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i32 @extractFloat64Exp(i64 %0) #2 !dbg !434 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !437, metadata !DIExpression()), !dbg !438
  %2 = lshr i64 %0, 52, !dbg !439
  %3 = and i64 %2, 2047, !dbg !440
  %4 = trunc i64 %3 to i32, !dbg !441
  ret i32 %4, !dbg !442
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i32 @extractFloat64Sign(i64 %0) #2 !dbg !443 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !444, metadata !DIExpression()), !dbg !445
  %2 = lshr i64 %0, 63, !dbg !446
  %3 = trunc i64 %2 to i32, !dbg !447
  ret i32 %3, !dbg !448
}

; Function Attrs: alwaysinline mustprogress uwtable
define dso_local void @_Z25normalizeFloat64SubnormalyPiPy(i64 %0, i32* %1, i64* %2) #3 !dbg !449 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !453, metadata !DIExpression()), !dbg !454
  call void @llvm.dbg.value(metadata i32* %1, metadata !455, metadata !DIExpression()), !dbg !454
  call void @llvm.dbg.value(metadata i64* %2, metadata !456, metadata !DIExpression()), !dbg !454
  %4 = call i32 @_ZL19countLeadingZeros64y(i64 %0), !dbg !457
  %5 = sub nsw i32 %4, 11, !dbg !458
  call void @llvm.dbg.value(metadata i32 %5, metadata !459, metadata !DIExpression()), !dbg !454
  %6 = zext i32 %5 to i64, !dbg !460
  %7 = shl i64 %0, %6, !dbg !460
  store i64 %7, i64* %2, align 8, !dbg !461
  %8 = sub nsw i32 1, %5, !dbg !462
  store i32 %8, i32* %1, align 4, !dbg !463
  ret void, !dbg !464
}

; Function Attrs: mustprogress noinline uwtable
define internal i32 @_ZL19countLeadingZeros64y(i64 %0) #4 !dbg !465 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !468, metadata !DIExpression()), !dbg !469
  call void @llvm.dbg.value(metadata i32 0, metadata !470, metadata !DIExpression()), !dbg !469
  %2 = icmp ult i64 %0, 4294967296, !dbg !471
  br i1 %2, label %3, label %5, !dbg !473

3:                                                ; preds = %1
  %4 = add nsw i32 0, 32, !dbg !474
  call void @llvm.dbg.value(metadata i32 %4, metadata !470, metadata !DIExpression()), !dbg !469
  br label %7, !dbg !476

5:                                                ; preds = %1
  %6 = lshr i64 %0, 32, !dbg !477
  call void @llvm.dbg.value(metadata i64 %6, metadata !468, metadata !DIExpression()), !dbg !469
  br label %7

7:                                                ; preds = %5, %3
  %.01 = phi i32 [ %4, %3 ], [ 0, %5 ], !dbg !469
  %.0 = phi i64 [ %0, %3 ], [ %6, %5 ]
  call void @llvm.dbg.value(metadata i64 %.0, metadata !468, metadata !DIExpression()), !dbg !469
  call void @llvm.dbg.value(metadata i32 %.01, metadata !470, metadata !DIExpression()), !dbg !469
  %8 = trunc i64 %.0 to i32, !dbg !479
  %9 = call i32 @_ZL19countLeadingZeros32j(i32 %8), !dbg !480
  %10 = add nsw i32 %.01, %9, !dbg !481
  call void @llvm.dbg.value(metadata i32 %10, metadata !470, metadata !DIExpression()), !dbg !469
  ret i32 %10, !dbg !482
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i64 @packFloat64(i32 %0, i32 %1, i64 %2) #2 !dbg !483 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !486, metadata !DIExpression()), !dbg !487
  call void @llvm.dbg.value(metadata i32 %1, metadata !488, metadata !DIExpression()), !dbg !487
  call void @llvm.dbg.value(metadata i64 %2, metadata !489, metadata !DIExpression()), !dbg !487
  %4 = sext i32 %0 to i64, !dbg !490
  %5 = shl i64 %4, 63, !dbg !491
  %6 = sext i32 %1 to i64, !dbg !492
  %7 = shl i64 %6, 52, !dbg !493
  %8 = add i64 %5, %7, !dbg !494
  %9 = add i64 %8, %2, !dbg !495
  ret i64 %9, !dbg !496
}

; Function Attrs: alwaysinline mustprogress nounwind uwtable
define dso_local i64 @roundAndPackFloat64(i32 %0, i32 %1, i64 %2) #2 !dbg !497 {
  %4 = alloca i64, align 8
  call void @llvm.dbg.value(metadata i32 %0, metadata !498, metadata !DIExpression()), !dbg !499
  call void @llvm.dbg.value(metadata i32 %1, metadata !500, metadata !DIExpression()), !dbg !499
  store i64 %2, i64* %4, align 8
  call void @llvm.dbg.declare(metadata i64* %4, metadata !501, metadata !DIExpression()), !dbg !502
  %5 = load i32, i32* @float_rounding_mode, align 4, !dbg !503
  call void @llvm.dbg.value(metadata i32 %5, metadata !504, metadata !DIExpression()), !dbg !499
  %6 = icmp eq i32 %5, 0, !dbg !505
  %7 = zext i1 %6 to i32, !dbg !506
  call void @llvm.dbg.value(metadata i32 %7, metadata !507, metadata !DIExpression()), !dbg !499
  call void @llvm.dbg.value(metadata i32 512, metadata !508, metadata !DIExpression()), !dbg !499
  %8 = icmp ne i32 %7, 0, !dbg !509
  br i1 %8, label %24, label %9, !dbg !511

9:                                                ; preds = %3
  %10 = icmp eq i32 %5, 1, !dbg !512
  br i1 %10, label %11, label %12, !dbg !515

11:                                               ; preds = %9
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !499
  br label %23, !dbg !516

12:                                               ; preds = %9
  call void @llvm.dbg.value(metadata i32 1023, metadata !508, metadata !DIExpression()), !dbg !499
  %13 = icmp ne i32 %0, 0, !dbg !518
  br i1 %13, label %14, label %18, !dbg !521

14:                                               ; preds = %12
  %15 = icmp eq i32 %5, 2, !dbg !522
  br i1 %15, label %16, label %17, !dbg !525

16:                                               ; preds = %14
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !499
  br label %17, !dbg !526

17:                                               ; preds = %16, %14
  %.01 = phi i32 [ 0, %16 ], [ 1023, %14 ], !dbg !527
  call void @llvm.dbg.value(metadata i32 %.01, metadata !508, metadata !DIExpression()), !dbg !499
  br label %22, !dbg !528

18:                                               ; preds = %12
  %19 = icmp eq i32 %5, 3, !dbg !529
  br i1 %19, label %20, label %21, !dbg !532

20:                                               ; preds = %18
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !499
  br label %21, !dbg !533

21:                                               ; preds = %20, %18
  %.12 = phi i32 [ 0, %20 ], [ 1023, %18 ], !dbg !527
  call void @llvm.dbg.value(metadata i32 %.12, metadata !508, metadata !DIExpression()), !dbg !499
  br label %22

22:                                               ; preds = %21, %17
  %.2 = phi i32 [ %.01, %17 ], [ %.12, %21 ], !dbg !534
  call void @llvm.dbg.value(metadata i32 %.2, metadata !508, metadata !DIExpression()), !dbg !499
  br label %23

23:                                               ; preds = %22, %11
  %.3 = phi i32 [ 0, %11 ], [ %.2, %22 ], !dbg !535
  call void @llvm.dbg.value(metadata i32 %.3, metadata !508, metadata !DIExpression()), !dbg !499
  br label %24, !dbg !536

24:                                               ; preds = %23, %3
  %.4 = phi i32 [ 512, %3 ], [ %.3, %23 ], !dbg !499
  call void @llvm.dbg.value(metadata i32 %.4, metadata !508, metadata !DIExpression()), !dbg !499
  %25 = load i64, i64* %4, align 8, !dbg !537
  %26 = and i64 %25, 1023, !dbg !538
  %27 = trunc i64 %26 to i32, !dbg !537
  call void @llvm.dbg.value(metadata i32 %27, metadata !539, metadata !DIExpression()), !dbg !499
  %28 = trunc i32 %1 to i16, !dbg !540
  %29 = zext i16 %28 to i32, !dbg !542
  %30 = icmp sle i32 2045, %29, !dbg !543
  br i1 %30, label %31, label %64, !dbg !544

31:                                               ; preds = %24
  %32 = icmp slt i32 2045, %1, !dbg !545
  br i1 %32, label %40, label %33, !dbg !548

33:                                               ; preds = %31
  %34 = icmp eq i32 %1, 2045, !dbg !549
  br i1 %34, label %35, label %50, !dbg !550

35:                                               ; preds = %33
  %36 = load i64, i64* %4, align 8, !dbg !551
  %37 = sext i32 %.4 to i64, !dbg !552
  %38 = add i64 %36, %37, !dbg !553
  %39 = icmp slt i64 %38, 0, !dbg !554
  br i1 %39, label %40, label %50, !dbg !555

40:                                               ; preds = %35, %31
  call void @_Z11float_raisei(i32 9), !dbg !556
  call void @llvm.dbg.value(metadata i32 %0, metadata !486, metadata !DIExpression()), !dbg !558
  call void @llvm.dbg.value(metadata i32 2047, metadata !488, metadata !DIExpression()), !dbg !558
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !558
  %41 = sext i32 %0 to i64, !dbg !560
  %42 = shl i64 %41, 63, !dbg !561
  %43 = sext i32 2047 to i64, !dbg !562
  %44 = shl i64 %43, 52, !dbg !563
  %45 = add i64 %42, %44, !dbg !564
  %46 = add i64 %45, 0, !dbg !565
  %47 = icmp eq i32 %.4, 0, !dbg !566
  %48 = zext i1 %47 to i64, !dbg !567
  %49 = sub i64 %46, %48, !dbg !568
  br label %93, !dbg !569

50:                                               ; preds = %35, %33
  %51 = icmp slt i32 %1, 0, !dbg !570
  br i1 %51, label %52, label %63, !dbg !572

52:                                               ; preds = %50
  call void @llvm.dbg.value(metadata i32 1, metadata !573, metadata !DIExpression()), !dbg !499
  %53 = load i64, i64* %4, align 8, !dbg !574
  %54 = sub nsw i32 0, %1, !dbg !576
  call void @_Z19shift64RightJammingyiPy(i64 %53, i32 %54, i64* %4), !dbg !577
  call void @llvm.dbg.value(metadata i32 0, metadata !500, metadata !DIExpression()), !dbg !499
  %55 = load i64, i64* %4, align 8, !dbg !578
  %56 = and i64 %55, 1023, !dbg !579
  %57 = trunc i64 %56 to i32, !dbg !578
  call void @llvm.dbg.value(metadata i32 %57, metadata !539, metadata !DIExpression()), !dbg !499
  %58 = icmp ne i32 1, 0, !dbg !580
  br i1 %58, label %59, label %62, !dbg !582

59:                                               ; preds = %52
  %60 = icmp ne i32 %57, 0, !dbg !583
  br i1 %60, label %61, label %62, !dbg !584

61:                                               ; preds = %59
  call void @_Z11float_raisei(i32 4), !dbg !585
  br label %62, !dbg !585

62:                                               ; preds = %61, %59, %52
  br label %63, !dbg !586

63:                                               ; preds = %62, %50
  %.03 = phi i32 [ 0, %62 ], [ %1, %50 ]
  %.0 = phi i32 [ %57, %62 ], [ %27, %50 ], !dbg !499
  call void @llvm.dbg.value(metadata i32 %.0, metadata !539, metadata !DIExpression()), !dbg !499
  call void @llvm.dbg.value(metadata i32 %.03, metadata !500, metadata !DIExpression()), !dbg !499
  br label %64, !dbg !587

64:                                               ; preds = %63, %24
  %.14 = phi i32 [ %.03, %63 ], [ %1, %24 ]
  %.1 = phi i32 [ %.0, %63 ], [ %27, %24 ], !dbg !499
  call void @llvm.dbg.value(metadata i32 %.1, metadata !539, metadata !DIExpression()), !dbg !499
  call void @llvm.dbg.value(metadata i32 %.14, metadata !500, metadata !DIExpression()), !dbg !499
  %65 = icmp ne i32 %.1, 0, !dbg !588
  br i1 %65, label %66, label %69, !dbg !590

66:                                               ; preds = %64
  %67 = load i32, i32* @float_exception_flags, align 4, !dbg !591
  %68 = or i32 %67, 1, !dbg !591
  store i32 %68, i32* @float_exception_flags, align 4, !dbg !591
  br label %69, !dbg !592

69:                                               ; preds = %66, %64
  %70 = load i64, i64* %4, align 8, !dbg !593
  %71 = sext i32 %.4 to i64, !dbg !594
  %72 = add i64 %70, %71, !dbg !595
  %73 = lshr i64 %72, 10, !dbg !596
  store i64 %73, i64* %4, align 8, !dbg !597
  %74 = xor i32 %.1, 512, !dbg !598
  %75 = icmp eq i32 %74, 0, !dbg !599
  %76 = zext i1 %75 to i32, !dbg !600
  %77 = and i32 %76, %7, !dbg !601
  %78 = xor i32 %77, -1, !dbg !602
  %79 = sext i32 %78 to i64, !dbg !602
  %80 = load i64, i64* %4, align 8, !dbg !603
  %81 = and i64 %80, %79, !dbg !603
  store i64 %81, i64* %4, align 8, !dbg !603
  %82 = load i64, i64* %4, align 8, !dbg !604
  %83 = icmp eq i64 %82, 0, !dbg !606
  br i1 %83, label %84, label %85, !dbg !607

84:                                               ; preds = %69
  call void @llvm.dbg.value(metadata i32 0, metadata !500, metadata !DIExpression()), !dbg !499
  br label %85, !dbg !608

85:                                               ; preds = %84, %69
  %.25 = phi i32 [ 0, %84 ], [ %.14, %69 ], !dbg !499
  call void @llvm.dbg.value(metadata i32 %.25, metadata !500, metadata !DIExpression()), !dbg !499
  %86 = load i64, i64* %4, align 8, !dbg !609
  call void @llvm.dbg.value(metadata i32 %0, metadata !486, metadata !DIExpression()), !dbg !610
  call void @llvm.dbg.value(metadata i32 %.25, metadata !488, metadata !DIExpression()), !dbg !610
  call void @llvm.dbg.value(metadata i64 %86, metadata !489, metadata !DIExpression()), !dbg !610
  %87 = sext i32 %0 to i64, !dbg !612
  %88 = shl i64 %87, 63, !dbg !613
  %89 = sext i32 %.25 to i64, !dbg !614
  %90 = shl i64 %89, 52, !dbg !615
  %91 = add i64 %88, %90, !dbg !616
  %92 = add i64 %91, %86, !dbg !617
  br label %93, !dbg !618

93:                                               ; preds = %85, %40
  %.06 = phi i64 [ %49, %40 ], [ %92, %85 ], !dbg !499
  ret i64 %.06, !dbg !619
}

; Function Attrs: mustprogress noinline uwtable
define dso_local i64 @float64_div(i64 %0, i64 %1) #4 !dbg !620 {
  %3 = alloca i64, align 8
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  call void @llvm.dbg.value(metadata i64 %0, metadata !625, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %1, metadata !627, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.declare(metadata i64* %4, metadata !628, metadata !DIExpression()), !dbg !629
  call void @llvm.dbg.declare(metadata i64* %5, metadata !630, metadata !DIExpression()), !dbg !631
  call void @llvm.dbg.declare(metadata i64* %6, metadata !632, metadata !DIExpression()), !dbg !633
  call void @llvm.dbg.declare(metadata i64* %7, metadata !634, metadata !DIExpression()), !dbg !635
  call void @llvm.dbg.value(metadata i64 %0, metadata !430, metadata !DIExpression()), !dbg !636
  %8 = and i64 %0, 4503599627370495, !dbg !638
  call void @llvm.dbg.value(metadata i64 %8, metadata !639, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %0, metadata !437, metadata !DIExpression()), !dbg !640
  %9 = lshr i64 %0, 52, !dbg !642
  %10 = and i64 %9, 2047, !dbg !643
  %11 = trunc i64 %10 to i32, !dbg !644
  call void @llvm.dbg.value(metadata i32 %11, metadata !645, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %0, metadata !444, metadata !DIExpression()), !dbg !646
  %12 = lshr i64 %0, 63, !dbg !648
  %13 = trunc i64 %12 to i32, !dbg !649
  call void @llvm.dbg.value(metadata i32 %13, metadata !650, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %1, metadata !430, metadata !DIExpression()), !dbg !651
  %14 = and i64 %1, 4503599627370495, !dbg !653
  call void @llvm.dbg.value(metadata i64 %14, metadata !654, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %1, metadata !437, metadata !DIExpression()), !dbg !655
  %15 = lshr i64 %1, 52, !dbg !657
  %16 = and i64 %15, 2047, !dbg !658
  %17 = trunc i64 %16 to i32, !dbg !659
  call void @llvm.dbg.value(metadata i32 %17, metadata !660, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %1, metadata !444, metadata !DIExpression()), !dbg !661
  %18 = lshr i64 %1, 63, !dbg !663
  %19 = trunc i64 %18 to i32, !dbg !664
  call void @llvm.dbg.value(metadata i32 %19, metadata !665, metadata !DIExpression()), !dbg !626
  %20 = xor i32 %13, %19, !dbg !666
  call void @llvm.dbg.value(metadata i32 %20, metadata !667, metadata !DIExpression()), !dbg !626
  %21 = icmp eq i32 %11, 2047, !dbg !668
  br i1 %21, label %22, label %40, !dbg !670

22:                                               ; preds = %2
  %23 = icmp ne i64 %8, 0, !dbg !671
  br i1 %23, label %24, label %26, !dbg !674

24:                                               ; preds = %22
  %25 = call i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1), !dbg !675
  br label %213, !dbg !676

26:                                               ; preds = %22
  %27 = icmp eq i32 %17, 2047, !dbg !677
  br i1 %27, label %28, label %33, !dbg !679

28:                                               ; preds = %26
  %29 = icmp ne i64 %14, 0, !dbg !680
  br i1 %29, label %30, label %32, !dbg !683

30:                                               ; preds = %28
  %31 = call i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1), !dbg !684
  br label %213, !dbg !685

32:                                               ; preds = %28
  call void @_Z11float_raisei(i32 16), !dbg !686
  br label %213, !dbg !687

33:                                               ; preds = %26
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !688
  call void @llvm.dbg.value(metadata i32 2047, metadata !488, metadata !DIExpression()), !dbg !688
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !688
  %34 = sext i32 %20 to i64, !dbg !690
  %35 = shl i64 %34, 63, !dbg !691
  %36 = sext i32 2047 to i64, !dbg !692
  %37 = shl i64 %36, 52, !dbg !693
  %38 = add i64 %35, %37, !dbg !694
  %39 = add i64 %38, 0, !dbg !695
  br label %213, !dbg !696

40:                                               ; preds = %2
  %41 = icmp eq i32 %17, 2047, !dbg !697
  br i1 %41, label %42, label %53, !dbg !699

42:                                               ; preds = %40
  %43 = icmp ne i64 %14, 0, !dbg !700
  br i1 %43, label %44, label %46, !dbg !703

44:                                               ; preds = %42
  %45 = call i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1), !dbg !704
  br label %213, !dbg !705

46:                                               ; preds = %42
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !706
  call void @llvm.dbg.value(metadata i32 0, metadata !488, metadata !DIExpression()), !dbg !706
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !706
  %47 = sext i32 %20 to i64, !dbg !708
  %48 = shl i64 %47, 63, !dbg !709
  %49 = sext i32 0 to i64, !dbg !710
  %50 = shl i64 %49, 52, !dbg !711
  %51 = add i64 %48, %50, !dbg !712
  %52 = add i64 %51, 0, !dbg !713
  br label %213, !dbg !714

53:                                               ; preds = %40
  %54 = icmp eq i32 %17, 0, !dbg !715
  br i1 %54, label %55, label %75, !dbg !717

55:                                               ; preds = %53
  %56 = icmp eq i64 %14, 0, !dbg !718
  br i1 %56, label %57, label %69, !dbg !721

57:                                               ; preds = %55
  %58 = sext i32 %11 to i64, !dbg !722
  %59 = or i64 %58, %8, !dbg !725
  %60 = icmp eq i64 %59, 0, !dbg !726
  br i1 %60, label %61, label %62, !dbg !727

61:                                               ; preds = %57
  call void @_Z11float_raisei(i32 16), !dbg !728
  br label %213, !dbg !730

62:                                               ; preds = %57
  call void @_Z11float_raisei(i32 2), !dbg !731
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !732
  call void @llvm.dbg.value(metadata i32 2047, metadata !488, metadata !DIExpression()), !dbg !732
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !732
  %63 = sext i32 %20 to i64, !dbg !734
  %64 = shl i64 %63, 63, !dbg !735
  %65 = sext i32 2047 to i64, !dbg !736
  %66 = shl i64 %65, 52, !dbg !737
  %67 = add i64 %64, %66, !dbg !738
  %68 = add i64 %67, 0, !dbg !739
  br label %213, !dbg !740

69:                                               ; preds = %55
  call void @llvm.dbg.value(metadata i64 %14, metadata !453, metadata !DIExpression()), !dbg !741
  call void @llvm.dbg.value(metadata i32* undef, metadata !455, metadata !DIExpression()), !dbg !741
  call void @llvm.dbg.value(metadata i64* undef, metadata !456, metadata !DIExpression()), !dbg !741
  %70 = call i32 @_ZL19countLeadingZeros64y(i64 %14), !dbg !743
  %71 = sub nsw i32 %70, 11, !dbg !744
  call void @llvm.dbg.value(metadata i32 %71, metadata !459, metadata !DIExpression()), !dbg !741
  %72 = zext i32 %71 to i64, !dbg !745
  %73 = shl i64 %14, %72, !dbg !745
  call void @llvm.dbg.value(metadata i64 %73, metadata !654, metadata !DIExpression()), !dbg !626
  %74 = sub nsw i32 1, %71, !dbg !746
  call void @llvm.dbg.value(metadata i32 %74, metadata !660, metadata !DIExpression()), !dbg !626
  br label %75, !dbg !747

75:                                               ; preds = %69, %53
  %.015 = phi i64 [ %73, %69 ], [ %14, %53 ], !dbg !626
  %.012 = phi i32 [ %74, %69 ], [ %17, %53 ], !dbg !626
  call void @llvm.dbg.value(metadata i32 %.012, metadata !660, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %.015, metadata !654, metadata !DIExpression()), !dbg !626
  %76 = icmp eq i32 %11, 0, !dbg !748
  br i1 %76, label %77, label %92, !dbg !750

77:                                               ; preds = %75
  %78 = icmp eq i64 %8, 0, !dbg !751
  br i1 %78, label %79, label %86, !dbg !754

79:                                               ; preds = %77
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !755
  call void @llvm.dbg.value(metadata i32 0, metadata !488, metadata !DIExpression()), !dbg !755
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !755
  %80 = sext i32 %20 to i64, !dbg !757
  %81 = shl i64 %80, 63, !dbg !758
  %82 = sext i32 0 to i64, !dbg !759
  %83 = shl i64 %82, 52, !dbg !760
  %84 = add i64 %81, %83, !dbg !761
  %85 = add i64 %84, 0, !dbg !762
  br label %213, !dbg !763

86:                                               ; preds = %77
  call void @llvm.dbg.value(metadata i64 %8, metadata !453, metadata !DIExpression()), !dbg !764
  call void @llvm.dbg.value(metadata i32* undef, metadata !455, metadata !DIExpression()), !dbg !764
  call void @llvm.dbg.value(metadata i64* undef, metadata !456, metadata !DIExpression()), !dbg !764
  %87 = call i32 @_ZL19countLeadingZeros64y(i64 %8), !dbg !766
  %88 = sub nsw i32 %87, 11, !dbg !767
  call void @llvm.dbg.value(metadata i32 %88, metadata !459, metadata !DIExpression()), !dbg !764
  %89 = zext i32 %88 to i64, !dbg !768
  %90 = shl i64 %8, %89, !dbg !768
  call void @llvm.dbg.value(metadata i64 %90, metadata !639, metadata !DIExpression()), !dbg !626
  %91 = sub nsw i32 1, %88, !dbg !769
  call void @llvm.dbg.value(metadata i32 %91, metadata !645, metadata !DIExpression()), !dbg !626
  br label %92, !dbg !770

92:                                               ; preds = %86, %75
  %.013 = phi i64 [ %90, %86 ], [ %8, %75 ], !dbg !626
  %.011 = phi i32 [ %91, %86 ], [ %11, %75 ], !dbg !626
  call void @llvm.dbg.value(metadata i32 %.011, metadata !645, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i64 %.013, metadata !639, metadata !DIExpression()), !dbg !626
  %93 = sub nsw i32 %.011, %.012, !dbg !771
  %94 = add nsw i32 %93, 1021, !dbg !772
  call void @llvm.dbg.value(metadata i32 %94, metadata !773, metadata !DIExpression()), !dbg !626
  %95 = or i64 %.013, 4503599627370496, !dbg !774
  %96 = shl i64 %95, 10, !dbg !775
  call void @llvm.dbg.value(metadata i64 %96, metadata !639, metadata !DIExpression()), !dbg !626
  %97 = or i64 %.015, 4503599627370496, !dbg !776
  %98 = shl i64 %97, 11, !dbg !777
  call void @llvm.dbg.value(metadata i64 %98, metadata !654, metadata !DIExpression()), !dbg !626
  %99 = add i64 %96, %96, !dbg !778
  %100 = icmp ule i64 %98, %99, !dbg !780
  br i1 %100, label %101, label %104, !dbg !781

101:                                              ; preds = %92
  %102 = lshr i64 %96, 1, !dbg !782
  call void @llvm.dbg.value(metadata i64 %102, metadata !639, metadata !DIExpression()), !dbg !626
  %103 = add nsw i32 %94, 1, !dbg !784
  call void @llvm.dbg.value(metadata i32 %103, metadata !773, metadata !DIExpression()), !dbg !626
  br label %104, !dbg !785

104:                                              ; preds = %101, %92
  %.114 = phi i64 [ %102, %101 ], [ %96, %92 ], !dbg !626
  %.01 = phi i32 [ %103, %101 ], [ %94, %92 ], !dbg !626
  call void @llvm.dbg.value(metadata i64 %.114, metadata !639, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i32 %.01, metadata !773, metadata !DIExpression()), !dbg !626
  %105 = call i64 @_ZL18estimateDiv128To64yyy(i64 %.114, i64 0, i64 %98), !dbg !786
  call void @llvm.dbg.value(metadata i64 %105, metadata !787, metadata !DIExpression()), !dbg !626
  %106 = and i64 %105, 511, !dbg !788
  %107 = icmp ule i64 %106, 2, !dbg !790
  br i1 %107, label %108, label %123, !dbg !791

108:                                              ; preds = %104
  call void @_Z10mul64To128yyPyS_(i64 %98, i64 %105, i64* %6, i64* %7), !dbg !792
  %109 = load i64, i64* %6, align 8, !dbg !794
  %110 = load i64, i64* %7, align 8, !dbg !795
  call void @_Z6sub128yyyyPyS_(i64 %.114, i64 0, i64 %109, i64 %110, i64* %4, i64* %5), !dbg !796
  br label %111, !dbg !797

111:                                              ; preds = %114, %108
  %.0 = phi i64 [ %105, %108 ], [ %115, %114 ], !dbg !626
  call void @llvm.dbg.value(metadata i64 %.0, metadata !787, metadata !DIExpression()), !dbg !626
  %112 = load i64, i64* %4, align 8, !dbg !798
  %113 = icmp slt i64 %112, 0, !dbg !799
  br i1 %113, label %114, label %118, !dbg !797

114:                                              ; preds = %111
  %115 = add i64 %.0, -1, !dbg !800
  call void @llvm.dbg.value(metadata i64 %115, metadata !787, metadata !DIExpression()), !dbg !626
  %116 = load i64, i64* %4, align 8, !dbg !802
  %117 = load i64, i64* %5, align 8, !dbg !803
  call void @_Z6add128yyyyPyS_(i64 %116, i64 %117, i64 0, i64 %98, i64* %4, i64* %5), !dbg !804
  br label %111, !dbg !797, !llvm.loop !805

118:                                              ; preds = %111
  %119 = load i64, i64* %5, align 8, !dbg !808
  %120 = icmp ne i64 %119, 0, !dbg !809
  %121 = zext i1 %120 to i64, !dbg !810
  %122 = or i64 %.0, %121, !dbg !811
  call void @llvm.dbg.value(metadata i64 %122, metadata !787, metadata !DIExpression()), !dbg !626
  br label %123, !dbg !812

123:                                              ; preds = %118, %104
  %.1 = phi i64 [ %122, %118 ], [ %105, %104 ], !dbg !626
  call void @llvm.dbg.value(metadata i64 %.1, metadata !787, metadata !DIExpression()), !dbg !626
  call void @llvm.dbg.value(metadata i32 %20, metadata !498, metadata !DIExpression()), !dbg !813
  call void @llvm.dbg.value(metadata i32 %.01, metadata !500, metadata !DIExpression()), !dbg !813
  store i64 %.1, i64* %3, align 8
  call void @llvm.dbg.declare(metadata i64* %3, metadata !501, metadata !DIExpression()) #5, !dbg !815
  %124 = load i32, i32* @float_rounding_mode, align 4, !dbg !816
  call void @llvm.dbg.value(metadata i32 %124, metadata !504, metadata !DIExpression()), !dbg !813
  %125 = icmp eq i32 %124, 0, !dbg !817
  %126 = zext i1 %125 to i32, !dbg !818
  call void @llvm.dbg.value(metadata i32 %126, metadata !507, metadata !DIExpression()), !dbg !813
  call void @llvm.dbg.value(metadata i32 512, metadata !508, metadata !DIExpression()), !dbg !813
  %127 = icmp ne i32 %126, 0, !dbg !819
  br i1 %127, label %143, label %128, !dbg !820

128:                                              ; preds = %123
  %129 = icmp eq i32 %124, 1, !dbg !821
  br i1 %129, label %130, label %131, !dbg !822

130:                                              ; preds = %128
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !813
  br label %142, !dbg !823

131:                                              ; preds = %128
  call void @llvm.dbg.value(metadata i32 1023, metadata !508, metadata !DIExpression()), !dbg !813
  %132 = icmp ne i32 %20, 0, !dbg !824
  br i1 %132, label %133, label %137, !dbg !825

133:                                              ; preds = %131
  %134 = icmp eq i32 %124, 2, !dbg !826
  br i1 %134, label %135, label %136, !dbg !827

135:                                              ; preds = %133
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !813
  br label %136, !dbg !828

136:                                              ; preds = %135, %133
  %.05 = phi i32 [ 0, %135 ], [ 1023, %133 ], !dbg !829
  call void @llvm.dbg.value(metadata i32 %.05, metadata !508, metadata !DIExpression()), !dbg !813
  br label %141, !dbg !830

137:                                              ; preds = %131
  %138 = icmp eq i32 %124, 3, !dbg !831
  br i1 %138, label %139, label %140, !dbg !832

139:                                              ; preds = %137
  call void @llvm.dbg.value(metadata i32 0, metadata !508, metadata !DIExpression()), !dbg !813
  br label %140, !dbg !833

140:                                              ; preds = %139, %137
  %.16 = phi i32 [ 0, %139 ], [ 1023, %137 ], !dbg !829
  call void @llvm.dbg.value(metadata i32 %.16, metadata !508, metadata !DIExpression()), !dbg !813
  br label %141

141:                                              ; preds = %140, %136
  %.2 = phi i32 [ %.05, %136 ], [ %.16, %140 ], !dbg !834
  call void @llvm.dbg.value(metadata i32 %.2, metadata !508, metadata !DIExpression()), !dbg !813
  br label %142

142:                                              ; preds = %141, %130
  %.3 = phi i32 [ 0, %130 ], [ %.2, %141 ], !dbg !835
  call void @llvm.dbg.value(metadata i32 %.3, metadata !508, metadata !DIExpression()), !dbg !813
  br label %143, !dbg !836

143:                                              ; preds = %142, %123
  %.4 = phi i32 [ 512, %123 ], [ %.3, %142 ], !dbg !813
  call void @llvm.dbg.value(metadata i32 %.4, metadata !508, metadata !DIExpression()), !dbg !813
  %144 = load i64, i64* %3, align 8, !dbg !837
  %145 = and i64 %144, 1023, !dbg !838
  %146 = trunc i64 %145 to i32, !dbg !837
  call void @llvm.dbg.value(metadata i32 %146, metadata !539, metadata !DIExpression()), !dbg !813
  %147 = trunc i32 %.01 to i16, !dbg !839
  %148 = zext i16 %147 to i32, !dbg !840
  %149 = icmp sle i32 2045, %148, !dbg !841
  br i1 %149, label %150, label %183, !dbg !842

150:                                              ; preds = %143
  %151 = icmp slt i32 2045, %.01, !dbg !843
  br i1 %151, label %159, label %152, !dbg !844

152:                                              ; preds = %150
  %153 = icmp eq i32 %.01, 2045, !dbg !845
  br i1 %153, label %154, label %169, !dbg !846

154:                                              ; preds = %152
  %155 = load i64, i64* %3, align 8, !dbg !847
  %156 = sext i32 %.4 to i64, !dbg !848
  %157 = add i64 %155, %156, !dbg !849
  %158 = icmp slt i64 %157, 0, !dbg !850
  br i1 %158, label %159, label %169, !dbg !851

159:                                              ; preds = %154, %150
  call void @_Z11float_raisei(i32 9) #5, !dbg !852
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !853
  call void @llvm.dbg.value(metadata i32 2047, metadata !488, metadata !DIExpression()), !dbg !853
  call void @llvm.dbg.value(metadata i64 0, metadata !489, metadata !DIExpression()), !dbg !853
  %160 = sext i32 %20 to i64, !dbg !855
  %161 = shl i64 %160, 63, !dbg !856
  %162 = sext i32 2047 to i64, !dbg !857
  %163 = shl i64 %162, 52, !dbg !858
  %164 = add i64 %161, %163, !dbg !859
  %165 = add i64 %164, 0, !dbg !860
  %166 = icmp eq i32 %.4, 0, !dbg !861
  %167 = zext i1 %166 to i64, !dbg !862
  %168 = sub i64 %165, %167, !dbg !863
  br label %212, !dbg !864

169:                                              ; preds = %154, %152
  %170 = icmp slt i32 %.01, 0, !dbg !865
  br i1 %170, label %171, label %182, !dbg !866

171:                                              ; preds = %169
  call void @llvm.dbg.value(metadata i32 1, metadata !573, metadata !DIExpression()), !dbg !813
  %172 = load i64, i64* %3, align 8, !dbg !867
  %173 = sub nsw i32 0, %.01, !dbg !868
  call void @_Z19shift64RightJammingyiPy(i64 %172, i32 %173, i64* %3) #5, !dbg !869
  call void @llvm.dbg.value(metadata i32 0, metadata !500, metadata !DIExpression()), !dbg !813
  %174 = load i64, i64* %3, align 8, !dbg !870
  %175 = and i64 %174, 1023, !dbg !871
  %176 = trunc i64 %175 to i32, !dbg !870
  call void @llvm.dbg.value(metadata i32 %176, metadata !539, metadata !DIExpression()), !dbg !813
  %177 = icmp ne i32 1, 0, !dbg !872
  br i1 %177, label %178, label %181, !dbg !873

178:                                              ; preds = %171
  %179 = icmp ne i32 %176, 0, !dbg !874
  br i1 %179, label %180, label %181, !dbg !875

180:                                              ; preds = %178
  call void @_Z11float_raisei(i32 4) #5, !dbg !876
  br label %181, !dbg !876

181:                                              ; preds = %180, %178, %171
  br label %182, !dbg !877

182:                                              ; preds = %181, %169
  %.07 = phi i32 [ 0, %181 ], [ %.01, %169 ]
  %.03 = phi i32 [ %176, %181 ], [ %146, %169 ], !dbg !813
  call void @llvm.dbg.value(metadata i32 %.03, metadata !539, metadata !DIExpression()), !dbg !813
  call void @llvm.dbg.value(metadata i32 %.07, metadata !500, metadata !DIExpression()), !dbg !813
  br label %183, !dbg !878

183:                                              ; preds = %182, %143
  %.18 = phi i32 [ %.07, %182 ], [ %.01, %143 ]
  %.14 = phi i32 [ %.03, %182 ], [ %146, %143 ], !dbg !813
  call void @llvm.dbg.value(metadata i32 %.14, metadata !539, metadata !DIExpression()), !dbg !813
  call void @llvm.dbg.value(metadata i32 %.18, metadata !500, metadata !DIExpression()), !dbg !813
  %184 = icmp ne i32 %.14, 0, !dbg !879
  br i1 %184, label %185, label %188, !dbg !880

185:                                              ; preds = %183
  %186 = load i32, i32* @float_exception_flags, align 4, !dbg !881
  %187 = or i32 %186, 1, !dbg !881
  store i32 %187, i32* @float_exception_flags, align 4, !dbg !881
  br label %188, !dbg !882

188:                                              ; preds = %185, %183
  %189 = load i64, i64* %3, align 8, !dbg !883
  %190 = sext i32 %.4 to i64, !dbg !884
  %191 = add i64 %189, %190, !dbg !885
  %192 = lshr i64 %191, 10, !dbg !886
  store i64 %192, i64* %3, align 8, !dbg !887
  %193 = xor i32 %.14, 512, !dbg !888
  %194 = icmp eq i32 %193, 0, !dbg !889
  %195 = zext i1 %194 to i32, !dbg !890
  %196 = and i32 %195, %126, !dbg !891
  %197 = xor i32 %196, -1, !dbg !892
  %198 = sext i32 %197 to i64, !dbg !892
  %199 = load i64, i64* %3, align 8, !dbg !893
  %200 = and i64 %199, %198, !dbg !893
  store i64 %200, i64* %3, align 8, !dbg !893
  %201 = load i64, i64* %3, align 8, !dbg !894
  %202 = icmp eq i64 %201, 0, !dbg !895
  br i1 %202, label %203, label %204, !dbg !896

203:                                              ; preds = %188
  call void @llvm.dbg.value(metadata i32 0, metadata !500, metadata !DIExpression()), !dbg !813
  br label %204, !dbg !897

204:                                              ; preds = %203, %188
  %.29 = phi i32 [ 0, %203 ], [ %.18, %188 ], !dbg !813
  call void @llvm.dbg.value(metadata i32 %.29, metadata !500, metadata !DIExpression()), !dbg !813
  %205 = load i64, i64* %3, align 8, !dbg !898
  call void @llvm.dbg.value(metadata i32 %20, metadata !486, metadata !DIExpression()), !dbg !899
  call void @llvm.dbg.value(metadata i32 %.29, metadata !488, metadata !DIExpression()), !dbg !899
  call void @llvm.dbg.value(metadata i64 %205, metadata !489, metadata !DIExpression()), !dbg !899
  %206 = sext i32 %20 to i64, !dbg !901
  %207 = shl i64 %206, 63, !dbg !902
  %208 = sext i32 %.29 to i64, !dbg !903
  %209 = shl i64 %208, 52, !dbg !904
  %210 = add i64 %207, %209, !dbg !905
  %211 = add i64 %210, %205, !dbg !906
  br label %212, !dbg !907

212:                                              ; preds = %204, %159
  %.010 = phi i64 [ %168, %159 ], [ %211, %204 ], !dbg !813
  br label %213, !dbg !908

213:                                              ; preds = %212, %79, %62, %61, %46, %44, %33, %32, %30, %24
  %.02 = phi i64 [ %25, %24 ], [ %31, %30 ], [ 9223372036854775807, %32 ], [ %39, %33 ], [ %45, %44 ], [ %52, %46 ], [ 9223372036854775807, %61 ], [ %68, %62 ], [ %85, %79 ], [ %.010, %212 ], !dbg !626
  ret i64 %.02, !dbg !909
}

; Function Attrs: mustprogress noinline nounwind uwtable
define internal i64 @_ZL19propagateFloat64NaNyy(i64 %0, i64 %1) #0 !dbg !910 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !913, metadata !DIExpression()), !dbg !914
  call void @llvm.dbg.value(metadata i64 %1, metadata !915, metadata !DIExpression()), !dbg !914
  %3 = call i32 @_Z14float64_is_nany(i64 %0), !dbg !916
  call void @llvm.dbg.value(metadata i32 %3, metadata !917, metadata !DIExpression()), !dbg !914
  %4 = call i32 @_Z24float64_is_signaling_nany(i64 %0), !dbg !918
  call void @llvm.dbg.value(metadata i32 %4, metadata !919, metadata !DIExpression()), !dbg !914
  %5 = call i32 @_Z14float64_is_nany(i64 %1), !dbg !920
  call void @llvm.dbg.value(metadata i32 %5, metadata !921, metadata !DIExpression()), !dbg !914
  %6 = call i32 @_Z24float64_is_signaling_nany(i64 %1), !dbg !922
  call void @llvm.dbg.value(metadata i32 %6, metadata !923, metadata !DIExpression()), !dbg !914
  %7 = or i64 %0, 2251799813685248, !dbg !924
  call void @llvm.dbg.value(metadata i64 %7, metadata !913, metadata !DIExpression()), !dbg !914
  %8 = or i64 %1, 2251799813685248, !dbg !925
  call void @llvm.dbg.value(metadata i64 %8, metadata !915, metadata !DIExpression()), !dbg !914
  %9 = or i32 %4, %6, !dbg !926
  %10 = icmp ne i32 %9, 0, !dbg !928
  br i1 %10, label %11, label %12, !dbg !929

11:                                               ; preds = %2
  call void @_Z11float_raisei(i32 16), !dbg !930
  br label %12, !dbg !930

12:                                               ; preds = %11, %2
  %13 = icmp ne i32 %6, 0, !dbg !931
  br i1 %13, label %14, label %15, !dbg !931

14:                                               ; preds = %12
  br label %26, !dbg !931

15:                                               ; preds = %12
  %16 = icmp ne i32 %4, 0, !dbg !932
  br i1 %16, label %17, label %18, !dbg !932

17:                                               ; preds = %15
  br label %24, !dbg !932

18:                                               ; preds = %15
  %19 = icmp ne i32 %5, 0, !dbg !933
  br i1 %19, label %20, label %21, !dbg !933

20:                                               ; preds = %18
  br label %22, !dbg !933

21:                                               ; preds = %18
  br label %22, !dbg !933

22:                                               ; preds = %21, %20
  %23 = phi i64 [ %8, %20 ], [ %7, %21 ], !dbg !933
  br label %24, !dbg !932

24:                                               ; preds = %22, %17
  %25 = phi i64 [ %7, %17 ], [ %23, %22 ], !dbg !932
  br label %26, !dbg !931

26:                                               ; preds = %24, %14
  %27 = phi i64 [ %8, %14 ], [ %25, %24 ], !dbg !931
  ret i64 %27, !dbg !934
}

; Function Attrs: mustprogress noinline nounwind uwtable
define internal i64 @_ZL18estimateDiv128To64yyy(i64 %0, i64 %1, i64 %2) #0 !dbg !935 {
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  call void @llvm.dbg.value(metadata i64 %0, metadata !938, metadata !DIExpression()), !dbg !939
  call void @llvm.dbg.value(metadata i64 %1, metadata !940, metadata !DIExpression()), !dbg !939
  call void @llvm.dbg.value(metadata i64 %2, metadata !941, metadata !DIExpression()), !dbg !939
  call void @llvm.dbg.declare(metadata i64* %4, metadata !942, metadata !DIExpression()), !dbg !943
  call void @llvm.dbg.declare(metadata i64* %5, metadata !944, metadata !DIExpression()), !dbg !945
  call void @llvm.dbg.declare(metadata i64* %6, metadata !946, metadata !DIExpression()), !dbg !947
  call void @llvm.dbg.declare(metadata i64* %7, metadata !948, metadata !DIExpression()), !dbg !949
  %8 = icmp ule i64 %2, %0, !dbg !950
  br i1 %8, label %9, label %10, !dbg !952

9:                                                ; preds = %3
  br label %46, !dbg !953

10:                                               ; preds = %3
  %11 = lshr i64 %2, 32, !dbg !954
  call void @llvm.dbg.value(metadata i64 %11, metadata !955, metadata !DIExpression()), !dbg !939
  %12 = shl i64 %11, 32, !dbg !956
  %13 = icmp ule i64 %12, %0, !dbg !957
  br i1 %13, label %14, label %15, !dbg !958

14:                                               ; preds = %10
  br label %18, !dbg !958

15:                                               ; preds = %10
  %16 = udiv i64 %0, %11, !dbg !959
  %17 = shl i64 %16, 32, !dbg !960
  br label %18, !dbg !958

18:                                               ; preds = %15, %14
  %19 = phi i64 [ -4294967296, %14 ], [ %17, %15 ], !dbg !958
  call void @llvm.dbg.value(metadata i64 %19, metadata !961, metadata !DIExpression()), !dbg !939
  call void @_Z10mul64To128yyPyS_(i64 %2, i64 %19, i64* %6, i64* %7), !dbg !962
  %20 = load i64, i64* %6, align 8, !dbg !963
  %21 = load i64, i64* %7, align 8, !dbg !964
  call void @_Z6sub128yyyyPyS_(i64 %0, i64 %1, i64 %20, i64 %21, i64* %4, i64* %5), !dbg !965
  br label %22, !dbg !966

22:                                               ; preds = %25, %18
  %.01 = phi i64 [ %19, %18 ], [ %26, %25 ], !dbg !939
  call void @llvm.dbg.value(metadata i64 %.01, metadata !961, metadata !DIExpression()), !dbg !939
  %23 = load i64, i64* %4, align 8, !dbg !967
  %24 = icmp slt i64 %23, 0, !dbg !968
  br i1 %24, label %25, label %30, !dbg !966

25:                                               ; preds = %22
  %26 = sub i64 %.01, 4294967296, !dbg !969
  call void @llvm.dbg.value(metadata i64 %26, metadata !961, metadata !DIExpression()), !dbg !939
  %27 = shl i64 %2, 32, !dbg !971
  call void @llvm.dbg.value(metadata i64 %27, metadata !972, metadata !DIExpression()), !dbg !939
  %28 = load i64, i64* %4, align 8, !dbg !973
  %29 = load i64, i64* %5, align 8, !dbg !974
  call void @_Z6add128yyyyPyS_(i64 %28, i64 %29, i64 %11, i64 %27, i64* %4, i64* %5), !dbg !975
  br label %22, !dbg !966, !llvm.loop !976

30:                                               ; preds = %22
  %31 = load i64, i64* %4, align 8, !dbg !978
  %32 = shl i64 %31, 32, !dbg !979
  %33 = load i64, i64* %5, align 8, !dbg !980
  %34 = lshr i64 %33, 32, !dbg !981
  %35 = or i64 %32, %34, !dbg !982
  store i64 %35, i64* %4, align 8, !dbg !983
  %36 = shl i64 %11, 32, !dbg !984
  %37 = load i64, i64* %4, align 8, !dbg !985
  %38 = icmp ule i64 %36, %37, !dbg !986
  br i1 %38, label %39, label %40, !dbg !987

39:                                               ; preds = %30
  br label %43, !dbg !987

40:                                               ; preds = %30
  %41 = load i64, i64* %4, align 8, !dbg !988
  %42 = udiv i64 %41, %11, !dbg !989
  br label %43, !dbg !987

43:                                               ; preds = %40, %39
  %44 = phi i64 [ 4294967295, %39 ], [ %42, %40 ], !dbg !987
  %45 = or i64 %.01, %44, !dbg !990
  call void @llvm.dbg.value(metadata i64 %45, metadata !961, metadata !DIExpression()), !dbg !939
  br label %46, !dbg !991

46:                                               ; preds = %43, %9
  %.0 = phi i64 [ -1, %9 ], [ %45, %43 ], !dbg !939
  ret i64 %.0, !dbg !992
}

; Function Attrs: mustprogress noinline nounwind uwtable
define internal i32 @_ZL19countLeadingZeros32j(i32 %0) #0 !dbg !20 {
  call void @llvm.dbg.value(metadata i32 %0, metadata !993, metadata !DIExpression()), !dbg !994
  call void @llvm.dbg.value(metadata i32 0, metadata !995, metadata !DIExpression()), !dbg !994
  %2 = icmp ult i32 %0, 65536, !dbg !996
  br i1 %2, label %3, label %6, !dbg !998

3:                                                ; preds = %1
  %4 = add nsw i32 0, 16, !dbg !999
  call void @llvm.dbg.value(metadata i32 %4, metadata !995, metadata !DIExpression()), !dbg !994
  %5 = shl i32 %0, 16, !dbg !1001
  call void @llvm.dbg.value(metadata i32 %5, metadata !993, metadata !DIExpression()), !dbg !994
  br label %6, !dbg !1002

6:                                                ; preds = %3, %1
  %.01 = phi i32 [ %4, %3 ], [ 0, %1 ], !dbg !994
  %.0 = phi i32 [ %5, %3 ], [ %0, %1 ]
  call void @llvm.dbg.value(metadata i32 %.0, metadata !993, metadata !DIExpression()), !dbg !994
  call void @llvm.dbg.value(metadata i32 %.01, metadata !995, metadata !DIExpression()), !dbg !994
  %7 = icmp ult i32 %.0, 16777216, !dbg !1003
  br i1 %7, label %8, label %11, !dbg !1005

8:                                                ; preds = %6
  %9 = add nsw i32 %.01, 8, !dbg !1006
  call void @llvm.dbg.value(metadata i32 %9, metadata !995, metadata !DIExpression()), !dbg !994
  %10 = shl i32 %.0, 8, !dbg !1008
  call void @llvm.dbg.value(metadata i32 %10, metadata !993, metadata !DIExpression()), !dbg !994
  br label %11, !dbg !1009

11:                                               ; preds = %8, %6
  %.12 = phi i32 [ %9, %8 ], [ %.01, %6 ], !dbg !994
  %.1 = phi i32 [ %10, %8 ], [ %.0, %6 ], !dbg !994
  call void @llvm.dbg.value(metadata i32 %.1, metadata !993, metadata !DIExpression()), !dbg !994
  call void @llvm.dbg.value(metadata i32 %.12, metadata !995, metadata !DIExpression()), !dbg !994
  %12 = lshr i32 %.1, 24, !dbg !1010
  %13 = zext i32 %12 to i64, !dbg !1011
  %14 = getelementptr inbounds [256 x i32], [256 x i32]* bitcast (<{ [128 x i32], [128 x i32] }>* @_ZZL19countLeadingZeros32jE21countLeadingZerosHigh to [256 x i32]*), i64 0, i64 %13, !dbg !1011
  %15 = load i32, i32* %14, align 4, !dbg !1011
  %16 = add nsw i32 %.12, %15, !dbg !1012
  call void @llvm.dbg.value(metadata i32 %16, metadata !995, metadata !DIExpression()), !dbg !994
  ret i32 %16, !dbg !1013
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #1

attributes #0 = { mustprogress noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { alwaysinline mustprogress nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { alwaysinline mustprogress uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { mustprogress noinline uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!272, !273, !274, !275, !276}
!llvm.ident = !{!277}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "float_rounding_mode", scope: !2, file: !3, line: 60, type: !16, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus_14, file: !3, producer: "Ubuntu clang version 13.0.1-2ubuntu2.2", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !5, globals: !13, imports: !30, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "dfdiv/float64_div.cpp", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!4 = !{}
!5 = !{!6, !9, !11}
!6 = !DIDerivedType(tag: DW_TAG_typedef, name: "bits64", file: !7, line: 70, baseType: !8)
!7 = !DIFile(filename: "dfdiv/include/SPARC-GCC.h", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!8 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!9 = !DIDerivedType(tag: DW_TAG_typedef, name: "bits16", file: !7, line: 68, baseType: !10)
!10 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!11 = !DIDerivedType(tag: DW_TAG_typedef, name: "sbits64", file: !7, line: 71, baseType: !12)
!12 = !DIBasicType(name: "long long int", size: 64, encoding: DW_ATE_signed)
!13 = !{!0, !14, !18}
!14 = !DIGlobalVariableExpression(var: !15, expr: !DIExpression())
!15 = distinct !DIGlobalVariable(name: "float_exception_flags", scope: !2, file: !3, line: 61, type: !16, isLocal: false, isDefinition: true)
!16 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8", file: !7, line: 59, baseType: !17)
!17 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!18 = !DIGlobalVariableExpression(var: !19, expr: !DIExpression())
!19 = distinct !DIGlobalVariable(name: "countLeadingZerosHigh", scope: !20, file: !21, line: 189, type: !26, isLocal: true, isDefinition: true)
!20 = distinct !DISubprogram(name: "countLeadingZeros32", linkageName: "_ZL19countLeadingZeros32j", scope: !21, file: !21, line: 187, type: !22, scopeLine: 188, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!21 = !DIFile(filename: "dfdiv/include/softfloat-macros", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
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
!312 = distinct !DISubprogram(name: "add128", linkageName: "_Z6add128yyyyPyS_", scope: !21, file: !21, line: 88, type: !313, scopeLine: 90, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!313 = !DISubroutineType(types: !314)
!314 = !{null, !6, !6, !6, !6, !282, !282}
!315 = !DILocalVariable(name: "a0", arg: 1, scope: !312, file: !21, line: 88, type: !6)
!316 = !DILocation(line: 0, scope: !312)
!317 = !DILocalVariable(name: "a1", arg: 2, scope: !312, file: !21, line: 88, type: !6)
!318 = !DILocalVariable(name: "b0", arg: 3, scope: !312, file: !21, line: 88, type: !6)
!319 = !DILocalVariable(name: "b1", arg: 4, scope: !312, file: !21, line: 88, type: !6)
!320 = !DILocalVariable(name: "z0Ptr", arg: 5, scope: !312, file: !21, line: 88, type: !282)
!321 = !DILocalVariable(name: "z1Ptr", arg: 6, scope: !312, file: !21, line: 89, type: !282)
!322 = !DILocation(line: 93, column: 11, scope: !312)
!323 = !DILocalVariable(name: "z1", scope: !312, file: !21, line: 91, type: !6)
!324 = !DILocation(line: 94, column: 10, scope: !312)
!325 = !DILocation(line: 95, column: 15, scope: !312)
!326 = !DILocation(line: 95, column: 26, scope: !312)
!327 = !DILocation(line: 95, column: 22, scope: !312)
!328 = !DILocation(line: 95, column: 20, scope: !312)
!329 = !DILocation(line: 95, column: 10, scope: !312)
!330 = !DILocation(line: 97, column: 1, scope: !312)
!331 = distinct !DISubprogram(name: "sub128", linkageName: "_Z6sub128yyyyPyS_", scope: !21, file: !21, line: 108, type: !313, scopeLine: 110, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!332 = !DILocalVariable(name: "a0", arg: 1, scope: !331, file: !21, line: 108, type: !6)
!333 = !DILocation(line: 0, scope: !331)
!334 = !DILocalVariable(name: "a1", arg: 2, scope: !331, file: !21, line: 108, type: !6)
!335 = !DILocalVariable(name: "b0", arg: 3, scope: !331, file: !21, line: 108, type: !6)
!336 = !DILocalVariable(name: "b1", arg: 4, scope: !331, file: !21, line: 108, type: !6)
!337 = !DILocalVariable(name: "z0Ptr", arg: 5, scope: !331, file: !21, line: 108, type: !282)
!338 = !DILocalVariable(name: "z1Ptr", arg: 6, scope: !331, file: !21, line: 109, type: !282)
!339 = !DILocation(line: 112, column: 15, scope: !331)
!340 = !DILocation(line: 112, column: 10, scope: !331)
!341 = !DILocation(line: 113, column: 15, scope: !331)
!342 = !DILocation(line: 113, column: 26, scope: !331)
!343 = !DILocation(line: 113, column: 22, scope: !331)
!344 = !DILocation(line: 113, column: 20, scope: !331)
!345 = !DILocation(line: 113, column: 10, scope: !331)
!346 = !DILocation(line: 115, column: 1, scope: !331)
!347 = distinct !DISubprogram(name: "mul64To128", linkageName: "_Z10mul64To128yyPyS_", scope: !21, file: !21, line: 124, type: !348, scopeLine: 125, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!348 = !DISubroutineType(types: !349)
!349 = !{null, !6, !6, !282, !282}
!350 = !DILocalVariable(name: "a", arg: 1, scope: !347, file: !21, line: 124, type: !6)
!351 = !DILocation(line: 0, scope: !347)
!352 = !DILocalVariable(name: "b", arg: 2, scope: !347, file: !21, line: 124, type: !6)
!353 = !DILocalVariable(name: "z0Ptr", arg: 3, scope: !347, file: !21, line: 124, type: !282)
!354 = !DILocalVariable(name: "z1Ptr", arg: 4, scope: !347, file: !21, line: 124, type: !282)
!355 = !DILocation(line: 129, column: 10, scope: !347)
!356 = !DILocalVariable(name: "aLow", scope: !347, file: !21, line: 126, type: !24)
!357 = !DILocation(line: 130, column: 13, scope: !347)
!358 = !DILocation(line: 130, column: 11, scope: !347)
!359 = !DILocalVariable(name: "aHigh", scope: !347, file: !21, line: 126, type: !24)
!360 = !DILocation(line: 131, column: 10, scope: !347)
!361 = !DILocalVariable(name: "bLow", scope: !347, file: !21, line: 126, type: !24)
!362 = !DILocation(line: 132, column: 13, scope: !347)
!363 = !DILocation(line: 132, column: 11, scope: !347)
!364 = !DILocalVariable(name: "bHigh", scope: !347, file: !21, line: 126, type: !24)
!365 = !DILocation(line: 133, column: 18, scope: !347)
!366 = !DILocation(line: 133, column: 26, scope: !347)
!367 = !DILocation(line: 133, column: 24, scope: !347)
!368 = !DILocalVariable(name: "z1", scope: !347, file: !21, line: 127, type: !6)
!369 = !DILocation(line: 134, column: 24, scope: !347)
!370 = !DILocation(line: 134, column: 32, scope: !347)
!371 = !DILocation(line: 134, column: 30, scope: !347)
!372 = !DILocalVariable(name: "zMiddleA", scope: !347, file: !21, line: 127, type: !6)
!373 = !DILocation(line: 135, column: 24, scope: !347)
!374 = !DILocation(line: 135, column: 33, scope: !347)
!375 = !DILocation(line: 135, column: 31, scope: !347)
!376 = !DILocalVariable(name: "zMiddleB", scope: !347, file: !21, line: 127, type: !6)
!377 = !DILocation(line: 136, column: 18, scope: !347)
!378 = !DILocation(line: 136, column: 27, scope: !347)
!379 = !DILocation(line: 136, column: 25, scope: !347)
!380 = !DILocalVariable(name: "z0", scope: !347, file: !21, line: 127, type: !6)
!381 = !DILocation(line: 137, column: 12, scope: !347)
!382 = !DILocation(line: 138, column: 30, scope: !347)
!383 = !DILocation(line: 138, column: 20, scope: !347)
!384 = !DILocation(line: 138, column: 43, scope: !347)
!385 = !DILocation(line: 138, column: 62, scope: !347)
!386 = !DILocation(line: 138, column: 50, scope: !347)
!387 = !DILocation(line: 138, column: 6, scope: !347)
!388 = !DILocation(line: 139, column: 12, scope: !347)
!389 = !DILocation(line: 140, column: 6, scope: !347)
!390 = !DILocation(line: 141, column: 13, scope: !347)
!391 = !DILocation(line: 141, column: 9, scope: !347)
!392 = !DILocation(line: 141, column: 6, scope: !347)
!393 = !DILocation(line: 142, column: 10, scope: !347)
!394 = !DILocation(line: 143, column: 10, scope: !347)
!395 = !DILocation(line: 145, column: 1, scope: !347)
!396 = distinct !DISubprogram(name: "float_raise", linkageName: "_Z11float_raisei", scope: !397, file: !397, line: 64, type: !398, scopeLine: 65, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!397 = !DIFile(filename: "dfdiv/include/softfloat-specialize", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!398 = !DISubroutineType(types: !399)
!399 = !{null, !16}
!400 = !DILocalVariable(name: "flags", arg: 1, scope: !396, file: !397, line: 64, type: !16)
!401 = !DILocation(line: 0, scope: !396)
!402 = !DILocation(line: 66, column: 25, scope: !396)
!403 = !DILocation(line: 68, column: 1, scope: !396)
!404 = distinct !DISubprogram(name: "float64_is_nan", linkageName: "_Z14float64_is_nany", scope: !397, file: !397, line: 82, type: !405, scopeLine: 83, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!405 = !DISubroutineType(types: !406)
!406 = !{!407, !408}
!407 = !DIDerivedType(tag: DW_TAG_typedef, name: "flag", file: !7, line: 58, baseType: !17)
!408 = !DIDerivedType(tag: DW_TAG_typedef, name: "float64", file: !409, line: 54, baseType: !8)
!409 = !DIFile(filename: "dfdiv/include/softfloat.h", directory: "/home/xyf/CoSense/applications/newton/llvm-ir/CHStone_test")
!410 = !DILocalVariable(name: "a", arg: 1, scope: !404, file: !397, line: 82, type: !408)
!411 = !DILocation(line: 0, scope: !404)
!412 = !DILocation(line: 85, column: 52, scope: !404)
!413 = !DILocation(line: 85, column: 38, scope: !404)
!414 = !DILocation(line: 85, column: 10, scope: !404)
!415 = !DILocation(line: 85, column: 3, scope: !404)
!416 = distinct !DISubprogram(name: "float64_is_signaling_nan", linkageName: "_Z24float64_is_signaling_nany", scope: !397, file: !397, line: 95, type: !405, scopeLine: 96, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!417 = !DILocalVariable(name: "a", arg: 1, scope: !416, file: !397, line: 95, type: !408)
!418 = !DILocation(line: 0, scope: !416)
!419 = !DILocation(line: 98, column: 15, scope: !416)
!420 = !DILocation(line: 98, column: 22, scope: !416)
!421 = !DILocation(line: 98, column: 31, scope: !416)
!422 = !DILocation(line: 98, column: 41, scope: !416)
!423 = !DILocation(line: 98, column: 47, scope: !416)
!424 = !DILocation(line: 98, column: 44, scope: !416)
!425 = !DILocation(line: 98, column: 10, scope: !416)
!426 = !DILocation(line: 98, column: 3, scope: !416)
!427 = distinct !DISubprogram(name: "extractFloat64Frac", scope: !3, file: !3, line: 87, type: !428, scopeLine: 88, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!428 = !DISubroutineType(types: !429)
!429 = !{!6, !408}
!430 = !DILocalVariable(name: "a", arg: 1, scope: !427, file: !3, line: 87, type: !408)
!431 = !DILocation(line: 0, scope: !427)
!432 = !DILocation(line: 89, column: 12, scope: !427)
!433 = !DILocation(line: 89, column: 3, scope: !427)
!434 = distinct !DISubprogram(name: "extractFloat64Exp", scope: !3, file: !3, line: 102, type: !435, scopeLine: 103, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!435 = !DISubroutineType(types: !436)
!436 = !{!281, !408}
!437 = !DILocalVariable(name: "a", arg: 1, scope: !434, file: !3, line: 102, type: !408)
!438 = !DILocation(line: 0, scope: !434)
!439 = !DILocation(line: 104, column: 13, scope: !434)
!440 = !DILocation(line: 104, column: 20, scope: !434)
!441 = !DILocation(line: 104, column: 10, scope: !434)
!442 = !DILocation(line: 104, column: 3, scope: !434)
!443 = distinct !DISubprogram(name: "extractFloat64Sign", scope: !3, file: !3, line: 117, type: !405, scopeLine: 118, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!444 = !DILocalVariable(name: "a", arg: 1, scope: !443, file: !3, line: 117, type: !408)
!445 = !DILocation(line: 0, scope: !443)
!446 = !DILocation(line: 120, column: 12, scope: !443)
!447 = !DILocation(line: 120, column: 10, scope: !443)
!448 = !DILocation(line: 120, column: 3, scope: !443)
!449 = distinct !DISubprogram(name: "normalizeFloat64Subnormal", linkageName: "_Z25normalizeFloat64SubnormalyPiPy", scope: !3, file: !3, line: 134, type: !450, scopeLine: 135, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!450 = !DISubroutineType(types: !451)
!451 = !{null, !6, !452, !282}
!452 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !281, size: 64)
!453 = !DILocalVariable(name: "aSig", arg: 1, scope: !449, file: !3, line: 134, type: !6)
!454 = !DILocation(line: 0, scope: !449)
!455 = !DILocalVariable(name: "zExpPtr", arg: 2, scope: !449, file: !3, line: 134, type: !452)
!456 = !DILocalVariable(name: "zSigPtr", arg: 3, scope: !449, file: !3, line: 134, type: !282)
!457 = !DILocation(line: 138, column: 16, scope: !449)
!458 = !DILocation(line: 138, column: 43, scope: !449)
!459 = !DILocalVariable(name: "shiftCount", scope: !449, file: !3, line: 136, type: !16)
!460 = !DILocation(line: 139, column: 19, scope: !449)
!461 = !DILocation(line: 139, column: 12, scope: !449)
!462 = !DILocation(line: 140, column: 16, scope: !449)
!463 = !DILocation(line: 140, column: 12, scope: !449)
!464 = !DILocation(line: 141, column: 1, scope: !449)
!465 = distinct !DISubprogram(name: "countLeadingZeros64", linkageName: "_ZL19countLeadingZeros64y", scope: !21, file: !21, line: 231, type: !466, scopeLine: 232, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!466 = !DISubroutineType(types: !467)
!467 = !{!16, !6}
!468 = !DILocalVariable(name: "a", arg: 1, scope: !465, file: !21, line: 231, type: !6)
!469 = !DILocation(line: 0, scope: !465)
!470 = !DILocalVariable(name: "shiftCount", scope: !465, file: !21, line: 233, type: !16)
!471 = !DILocation(line: 236, column: 9, scope: !472)
!472 = distinct !DILexicalBlock(scope: !465, file: !21, line: 236, column: 7)
!473 = !DILocation(line: 236, column: 7, scope: !465)
!474 = !DILocation(line: 238, column: 18, scope: !475)
!475 = distinct !DILexicalBlock(scope: !472, file: !21, line: 237, column: 5)
!476 = !DILocation(line: 239, column: 5, scope: !475)
!477 = !DILocation(line: 242, column: 9, scope: !478)
!478 = distinct !DILexicalBlock(scope: !472, file: !21, line: 241, column: 5)
!479 = !DILocation(line: 244, column: 38, scope: !465)
!480 = !DILocation(line: 244, column: 17, scope: !465)
!481 = !DILocation(line: 244, column: 14, scope: !465)
!482 = !DILocation(line: 245, column: 3, scope: !465)
!483 = distinct !DISubprogram(name: "packFloat64", scope: !3, file: !3, line: 157, type: !484, scopeLine: 158, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!484 = !DISubroutineType(types: !485)
!485 = !{!408, !407, !281, !6}
!486 = !DILocalVariable(name: "zSign", arg: 1, scope: !483, file: !3, line: 157, type: !407)
!487 = !DILocation(line: 0, scope: !483)
!488 = !DILocalVariable(name: "zExp", arg: 2, scope: !483, file: !3, line: 157, type: !281)
!489 = !DILocalVariable(name: "zSig", arg: 3, scope: !483, file: !3, line: 157, type: !6)
!490 = !DILocation(line: 159, column: 21, scope: !483)
!491 = !DILocation(line: 159, column: 28, scope: !483)
!492 = !DILocation(line: 159, column: 48, scope: !483)
!493 = !DILocation(line: 159, column: 54, scope: !483)
!494 = !DILocation(line: 159, column: 35, scope: !483)
!495 = !DILocation(line: 159, column: 61, scope: !483)
!496 = !DILocation(line: 159, column: 3, scope: !483)
!497 = distinct !DISubprogram(name: "roundAndPackFloat64", scope: !3, file: !3, line: 190, type: !484, scopeLine: 191, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!498 = !DILocalVariable(name: "zSign", arg: 1, scope: !497, file: !3, line: 190, type: !407)
!499 = !DILocation(line: 0, scope: !497)
!500 = !DILocalVariable(name: "zExp", arg: 2, scope: !497, file: !3, line: 190, type: !281)
!501 = !DILocalVariable(name: "zSig", arg: 3, scope: !497, file: !3, line: 190, type: !6)
!502 = !DILocation(line: 190, column: 61, scope: !497)
!503 = !DILocation(line: 196, column: 18, scope: !497)
!504 = !DILocalVariable(name: "roundingMode", scope: !497, file: !3, line: 192, type: !16)
!505 = !DILocation(line: 197, column: 36, scope: !497)
!506 = !DILocation(line: 197, column: 22, scope: !497)
!507 = !DILocalVariable(name: "roundNearestEven", scope: !497, file: !3, line: 193, type: !407)
!508 = !DILocalVariable(name: "roundIncrement", scope: !497, file: !3, line: 194, type: !281)
!509 = !DILocation(line: 199, column: 8, scope: !510)
!510 = distinct !DILexicalBlock(scope: !497, file: !3, line: 199, column: 7)
!511 = !DILocation(line: 199, column: 7, scope: !497)
!512 = !DILocation(line: 201, column: 24, scope: !513)
!513 = distinct !DILexicalBlock(scope: !514, file: !3, line: 201, column: 11)
!514 = distinct !DILexicalBlock(scope: !510, file: !3, line: 200, column: 5)
!515 = !DILocation(line: 201, column: 11, scope: !514)
!516 = !DILocation(line: 204, column: 2, scope: !517)
!517 = distinct !DILexicalBlock(scope: !513, file: !3, line: 202, column: 2)
!518 = !DILocation(line: 208, column: 8, scope: !519)
!519 = distinct !DILexicalBlock(scope: !520, file: !3, line: 208, column: 8)
!520 = distinct !DILexicalBlock(scope: !513, file: !3, line: 206, column: 2)
!521 = !DILocation(line: 208, column: 8, scope: !520)
!522 = !DILocation(line: 210, column: 25, scope: !523)
!523 = distinct !DILexicalBlock(scope: !524, file: !3, line: 210, column: 12)
!524 = distinct !DILexicalBlock(scope: !519, file: !3, line: 209, column: 6)
!525 = !DILocation(line: 210, column: 12, scope: !524)
!526 = !DILocation(line: 211, column: 3, scope: !523)
!527 = !DILocation(line: 0, scope: !520)
!528 = !DILocation(line: 212, column: 6, scope: !524)
!529 = !DILocation(line: 215, column: 25, scope: !530)
!530 = distinct !DILexicalBlock(scope: !531, file: !3, line: 215, column: 12)
!531 = distinct !DILexicalBlock(scope: !519, file: !3, line: 214, column: 6)
!532 = !DILocation(line: 215, column: 12, scope: !531)
!533 = !DILocation(line: 216, column: 3, scope: !530)
!534 = !DILocation(line: 0, scope: !519)
!535 = !DILocation(line: 0, scope: !513)
!536 = !DILocation(line: 219, column: 5, scope: !514)
!537 = !DILocation(line: 220, column: 15, scope: !497)
!538 = !DILocation(line: 220, column: 20, scope: !497)
!539 = !DILocalVariable(name: "roundBits", scope: !497, file: !3, line: 194, type: !281)
!540 = !DILocation(line: 221, column: 25, scope: !541)
!541 = distinct !DILexicalBlock(scope: !497, file: !3, line: 221, column: 7)
!542 = !DILocation(line: 221, column: 16, scope: !541)
!543 = !DILocation(line: 221, column: 13, scope: !541)
!544 = !DILocation(line: 221, column: 7, scope: !497)
!545 = !DILocation(line: 223, column: 18, scope: !546)
!546 = distinct !DILexicalBlock(scope: !547, file: !3, line: 223, column: 11)
!547 = distinct !DILexicalBlock(scope: !541, file: !3, line: 222, column: 5)
!548 = !DILocation(line: 224, column: 4, scope: !546)
!549 = !DILocation(line: 224, column: 14, scope: !546)
!550 = !DILocation(line: 224, column: 24, scope: !546)
!551 = !DILocation(line: 224, column: 39, scope: !546)
!552 = !DILocation(line: 224, column: 46, scope: !546)
!553 = !DILocation(line: 224, column: 44, scope: !546)
!554 = !DILocation(line: 224, column: 62, scope: !546)
!555 = !DILocation(line: 223, column: 11, scope: !547)
!556 = !DILocation(line: 226, column: 4, scope: !557)
!557 = distinct !DILexicalBlock(scope: !546, file: !3, line: 225, column: 2)
!558 = !DILocation(line: 0, scope: !483, inlinedAt: !559)
!559 = distinct !DILocation(line: 227, column: 11, scope: !557)
!560 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !559)
!561 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !559)
!562 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !559)
!563 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !559)
!564 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !559)
!565 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !559)
!566 = !DILocation(line: 227, column: 59, scope: !557)
!567 = !DILocation(line: 227, column: 43, scope: !557)
!568 = !DILocation(line: 227, column: 41, scope: !557)
!569 = !DILocation(line: 227, column: 4, scope: !557)
!570 = !DILocation(line: 229, column: 16, scope: !571)
!571 = distinct !DILexicalBlock(scope: !547, file: !3, line: 229, column: 11)
!572 = !DILocation(line: 229, column: 11, scope: !547)
!573 = !DILocalVariable(name: "isTiny", scope: !497, file: !3, line: 193, type: !407)
!574 = !DILocation(line: 234, column: 25, scope: !575)
!575 = distinct !DILexicalBlock(scope: !571, file: !3, line: 230, column: 2)
!576 = !DILocation(line: 234, column: 31, scope: !575)
!577 = !DILocation(line: 234, column: 4, scope: !575)
!578 = !DILocation(line: 236, column: 16, scope: !575)
!579 = !DILocation(line: 236, column: 21, scope: !575)
!580 = !DILocation(line: 237, column: 8, scope: !581)
!581 = distinct !DILexicalBlock(scope: !575, file: !3, line: 237, column: 8)
!582 = !DILocation(line: 237, column: 15, scope: !581)
!583 = !DILocation(line: 237, column: 18, scope: !581)
!584 = !DILocation(line: 237, column: 8, scope: !575)
!585 = !DILocation(line: 238, column: 6, scope: !581)
!586 = !DILocation(line: 239, column: 2, scope: !575)
!587 = !DILocation(line: 240, column: 5, scope: !547)
!588 = !DILocation(line: 241, column: 7, scope: !589)
!589 = distinct !DILexicalBlock(scope: !497, file: !3, line: 241, column: 7)
!590 = !DILocation(line: 241, column: 7, scope: !497)
!591 = !DILocation(line: 242, column: 27, scope: !589)
!592 = !DILocation(line: 242, column: 5, scope: !589)
!593 = !DILocation(line: 243, column: 11, scope: !497)
!594 = !DILocation(line: 243, column: 18, scope: !497)
!595 = !DILocation(line: 243, column: 16, scope: !497)
!596 = !DILocation(line: 243, column: 34, scope: !497)
!597 = !DILocation(line: 243, column: 8, scope: !497)
!598 = !DILocation(line: 244, column: 25, scope: !497)
!599 = !DILocation(line: 244, column: 34, scope: !497)
!600 = !DILocation(line: 244, column: 13, scope: !497)
!601 = !DILocation(line: 244, column: 40, scope: !497)
!602 = !DILocation(line: 244, column: 11, scope: !497)
!603 = !DILocation(line: 244, column: 8, scope: !497)
!604 = !DILocation(line: 245, column: 7, scope: !605)
!605 = distinct !DILexicalBlock(scope: !497, file: !3, line: 245, column: 7)
!606 = !DILocation(line: 245, column: 12, scope: !605)
!607 = !DILocation(line: 245, column: 7, scope: !497)
!608 = !DILocation(line: 246, column: 5, scope: !605)
!609 = !DILocation(line: 247, column: 36, scope: !497)
!610 = !DILocation(line: 0, scope: !483, inlinedAt: !611)
!611 = distinct !DILocation(line: 247, column: 10, scope: !497)
!612 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !611)
!613 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !611)
!614 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !611)
!615 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !611)
!616 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !611)
!617 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !611)
!618 = !DILocation(line: 247, column: 3, scope: !497)
!619 = !DILocation(line: 249, column: 1, scope: !497)
!620 = distinct !DISubprogram(name: "float64_div", scope: !3, file: !3, line: 273, type: !621, scopeLine: 274, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!621 = !DISubroutineType(types: !622)
!622 = !{!408, !623, !624}
!623 = !DIDerivedType(tag: DW_TAG_typedef, name: "bmx055xAcceleration", file: !3, line: 263, baseType: !408)
!624 = !DIDerivedType(tag: DW_TAG_typedef, name: "bmx055yAcceleration", file: !3, line: 264, baseType: !408)
!625 = !DILocalVariable(name: "a", arg: 1, scope: !620, file: !3, line: 273, type: !623)
!626 = !DILocation(line: 0, scope: !620)
!627 = !DILocalVariable(name: "b", arg: 2, scope: !620, file: !3, line: 273, type: !624)
!628 = !DILocalVariable(name: "rem0", scope: !620, file: !3, line: 289, type: !6)
!629 = !DILocation(line: 289, column: 10, scope: !620)
!630 = !DILocalVariable(name: "rem1", scope: !620, file: !3, line: 289, type: !6)
!631 = !DILocation(line: 289, column: 16, scope: !620)
!632 = !DILocalVariable(name: "term0", scope: !620, file: !3, line: 289, type: !6)
!633 = !DILocation(line: 289, column: 22, scope: !620)
!634 = !DILocalVariable(name: "term1", scope: !620, file: !3, line: 289, type: !6)
!635 = !DILocation(line: 289, column: 29, scope: !620)
!636 = !DILocation(line: 0, scope: !427, inlinedAt: !637)
!637 = distinct !DILocation(line: 291, column: 10, scope: !620)
!638 = !DILocation(line: 89, column: 12, scope: !427, inlinedAt: !637)
!639 = !DILocalVariable(name: "aSig", scope: !620, file: !3, line: 288, type: !6)
!640 = !DILocation(line: 0, scope: !434, inlinedAt: !641)
!641 = distinct !DILocation(line: 292, column: 10, scope: !620)
!642 = !DILocation(line: 104, column: 13, scope: !434, inlinedAt: !641)
!643 = !DILocation(line: 104, column: 20, scope: !434, inlinedAt: !641)
!644 = !DILocation(line: 104, column: 10, scope: !434, inlinedAt: !641)
!645 = !DILocalVariable(name: "aExp", scope: !620, file: !3, line: 287, type: !281)
!646 = !DILocation(line: 0, scope: !443, inlinedAt: !647)
!647 = distinct !DILocation(line: 293, column: 11, scope: !620)
!648 = !DILocation(line: 120, column: 12, scope: !443, inlinedAt: !647)
!649 = !DILocation(line: 120, column: 10, scope: !443, inlinedAt: !647)
!650 = !DILocalVariable(name: "aSign", scope: !620, file: !3, line: 286, type: !407)
!651 = !DILocation(line: 0, scope: !427, inlinedAt: !652)
!652 = distinct !DILocation(line: 294, column: 10, scope: !620)
!653 = !DILocation(line: 89, column: 12, scope: !427, inlinedAt: !652)
!654 = !DILocalVariable(name: "bSig", scope: !620, file: !3, line: 288, type: !6)
!655 = !DILocation(line: 0, scope: !434, inlinedAt: !656)
!656 = distinct !DILocation(line: 295, column: 10, scope: !620)
!657 = !DILocation(line: 104, column: 13, scope: !434, inlinedAt: !656)
!658 = !DILocation(line: 104, column: 20, scope: !434, inlinedAt: !656)
!659 = !DILocation(line: 104, column: 10, scope: !434, inlinedAt: !656)
!660 = !DILocalVariable(name: "bExp", scope: !620, file: !3, line: 287, type: !281)
!661 = !DILocation(line: 0, scope: !443, inlinedAt: !662)
!662 = distinct !DILocation(line: 296, column: 11, scope: !620)
!663 = !DILocation(line: 120, column: 12, scope: !443, inlinedAt: !662)
!664 = !DILocation(line: 120, column: 10, scope: !443, inlinedAt: !662)
!665 = !DILocalVariable(name: "bSign", scope: !620, file: !3, line: 286, type: !407)
!666 = !DILocation(line: 297, column: 17, scope: !620)
!667 = !DILocalVariable(name: "zSign", scope: !620, file: !3, line: 286, type: !407)
!668 = !DILocation(line: 298, column: 12, scope: !669)
!669 = distinct !DILexicalBlock(scope: !620, file: !3, line: 298, column: 7)
!670 = !DILocation(line: 298, column: 7, scope: !620)
!671 = !DILocation(line: 300, column: 11, scope: !672)
!672 = distinct !DILexicalBlock(scope: !673, file: !3, line: 300, column: 11)
!673 = distinct !DILexicalBlock(scope: !669, file: !3, line: 299, column: 5)
!674 = !DILocation(line: 300, column: 11, scope: !673)
!675 = !DILocation(line: 301, column: 9, scope: !672)
!676 = !DILocation(line: 301, column: 2, scope: !672)
!677 = !DILocation(line: 302, column: 16, scope: !678)
!678 = distinct !DILexicalBlock(scope: !673, file: !3, line: 302, column: 11)
!679 = !DILocation(line: 302, column: 11, scope: !673)
!680 = !DILocation(line: 304, column: 8, scope: !681)
!681 = distinct !DILexicalBlock(scope: !682, file: !3, line: 304, column: 8)
!682 = distinct !DILexicalBlock(scope: !678, file: !3, line: 303, column: 2)
!683 = !DILocation(line: 304, column: 8, scope: !682)
!684 = !DILocation(line: 305, column: 13, scope: !681)
!685 = !DILocation(line: 305, column: 6, scope: !681)
!686 = !DILocation(line: 306, column: 4, scope: !682)
!687 = !DILocation(line: 307, column: 4, scope: !682)
!688 = !DILocation(line: 0, scope: !483, inlinedAt: !689)
!689 = distinct !DILocation(line: 309, column: 14, scope: !673)
!690 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !689)
!691 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !689)
!692 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !689)
!693 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !689)
!694 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !689)
!695 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !689)
!696 = !DILocation(line: 309, column: 7, scope: !673)
!697 = !DILocation(line: 311, column: 12, scope: !698)
!698 = distinct !DILexicalBlock(scope: !620, file: !3, line: 311, column: 7)
!699 = !DILocation(line: 311, column: 7, scope: !620)
!700 = !DILocation(line: 313, column: 11, scope: !701)
!701 = distinct !DILexicalBlock(scope: !702, file: !3, line: 313, column: 11)
!702 = distinct !DILexicalBlock(scope: !698, file: !3, line: 312, column: 5)
!703 = !DILocation(line: 313, column: 11, scope: !702)
!704 = !DILocation(line: 314, column: 9, scope: !701)
!705 = !DILocation(line: 314, column: 2, scope: !701)
!706 = !DILocation(line: 0, scope: !483, inlinedAt: !707)
!707 = distinct !DILocation(line: 315, column: 14, scope: !702)
!708 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !707)
!709 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !707)
!710 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !707)
!711 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !707)
!712 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !707)
!713 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !707)
!714 = !DILocation(line: 315, column: 7, scope: !702)
!715 = !DILocation(line: 317, column: 12, scope: !716)
!716 = distinct !DILexicalBlock(scope: !620, file: !3, line: 317, column: 7)
!717 = !DILocation(line: 317, column: 7, scope: !620)
!718 = !DILocation(line: 319, column: 16, scope: !719)
!719 = distinct !DILexicalBlock(scope: !720, file: !3, line: 319, column: 11)
!720 = distinct !DILexicalBlock(scope: !716, file: !3, line: 318, column: 5)
!721 = !DILocation(line: 319, column: 11, scope: !720)
!722 = !DILocation(line: 321, column: 9, scope: !723)
!723 = distinct !DILexicalBlock(scope: !724, file: !3, line: 321, column: 8)
!724 = distinct !DILexicalBlock(scope: !719, file: !3, line: 320, column: 2)
!725 = !DILocation(line: 321, column: 14, scope: !723)
!726 = !DILocation(line: 321, column: 22, scope: !723)
!727 = !DILocation(line: 321, column: 8, scope: !724)
!728 = !DILocation(line: 323, column: 8, scope: !729)
!729 = distinct !DILexicalBlock(scope: !723, file: !3, line: 322, column: 6)
!730 = !DILocation(line: 324, column: 8, scope: !729)
!731 = !DILocation(line: 326, column: 4, scope: !724)
!732 = !DILocation(line: 0, scope: !483, inlinedAt: !733)
!733 = distinct !DILocation(line: 327, column: 11, scope: !724)
!734 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !733)
!735 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !733)
!736 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !733)
!737 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !733)
!738 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !733)
!739 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !733)
!740 = !DILocation(line: 327, column: 4, scope: !724)
!741 = !DILocation(line: 0, scope: !449, inlinedAt: !742)
!742 = distinct !DILocation(line: 329, column: 7, scope: !720)
!743 = !DILocation(line: 138, column: 16, scope: !449, inlinedAt: !742)
!744 = !DILocation(line: 138, column: 43, scope: !449, inlinedAt: !742)
!745 = !DILocation(line: 139, column: 19, scope: !449, inlinedAt: !742)
!746 = !DILocation(line: 140, column: 16, scope: !449, inlinedAt: !742)
!747 = !DILocation(line: 330, column: 5, scope: !720)
!748 = !DILocation(line: 331, column: 12, scope: !749)
!749 = distinct !DILexicalBlock(scope: !620, file: !3, line: 331, column: 7)
!750 = !DILocation(line: 331, column: 7, scope: !620)
!751 = !DILocation(line: 333, column: 16, scope: !752)
!752 = distinct !DILexicalBlock(scope: !753, file: !3, line: 333, column: 11)
!753 = distinct !DILexicalBlock(scope: !749, file: !3, line: 332, column: 5)
!754 = !DILocation(line: 333, column: 11, scope: !753)
!755 = !DILocation(line: 0, scope: !483, inlinedAt: !756)
!756 = distinct !DILocation(line: 334, column: 9, scope: !752)
!757 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !756)
!758 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !756)
!759 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !756)
!760 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !756)
!761 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !756)
!762 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !756)
!763 = !DILocation(line: 334, column: 2, scope: !752)
!764 = !DILocation(line: 0, scope: !449, inlinedAt: !765)
!765 = distinct !DILocation(line: 335, column: 7, scope: !753)
!766 = !DILocation(line: 138, column: 16, scope: !449, inlinedAt: !765)
!767 = !DILocation(line: 138, column: 43, scope: !449, inlinedAt: !765)
!768 = !DILocation(line: 139, column: 19, scope: !449, inlinedAt: !765)
!769 = !DILocation(line: 140, column: 16, scope: !449, inlinedAt: !765)
!770 = !DILocation(line: 336, column: 5, scope: !753)
!771 = !DILocation(line: 337, column: 15, scope: !620)
!772 = !DILocation(line: 337, column: 22, scope: !620)
!773 = !DILocalVariable(name: "zExp", scope: !620, file: !3, line: 287, type: !281)
!774 = !DILocation(line: 338, column: 16, scope: !620)
!775 = !DILocation(line: 338, column: 46, scope: !620)
!776 = !DILocation(line: 339, column: 16, scope: !620)
!777 = !DILocation(line: 339, column: 46, scope: !620)
!778 = !DILocation(line: 340, column: 21, scope: !779)
!779 = distinct !DILexicalBlock(scope: !620, file: !3, line: 340, column: 7)
!780 = !DILocation(line: 340, column: 12, scope: !779)
!781 = !DILocation(line: 340, column: 7, scope: !620)
!782 = !DILocation(line: 342, column: 12, scope: !783)
!783 = distinct !DILexicalBlock(scope: !779, file: !3, line: 341, column: 5)
!784 = !DILocation(line: 343, column: 7, scope: !783)
!785 = !DILocation(line: 344, column: 5, scope: !783)
!786 = !DILocation(line: 345, column: 10, scope: !620)
!787 = !DILocalVariable(name: "zSig", scope: !620, file: !3, line: 288, type: !6)
!788 = !DILocation(line: 346, column: 13, scope: !789)
!789 = distinct !DILexicalBlock(scope: !620, file: !3, line: 346, column: 7)
!790 = !DILocation(line: 346, column: 22, scope: !789)
!791 = !DILocation(line: 346, column: 7, scope: !620)
!792 = !DILocation(line: 348, column: 7, scope: !793)
!793 = distinct !DILexicalBlock(scope: !789, file: !3, line: 347, column: 5)
!794 = !DILocation(line: 349, column: 24, scope: !793)
!795 = !DILocation(line: 349, column: 31, scope: !793)
!796 = !DILocation(line: 349, column: 7, scope: !793)
!797 = !DILocation(line: 350, column: 7, scope: !793)
!798 = !DILocation(line: 350, column: 24, scope: !793)
!799 = !DILocation(line: 350, column: 29, scope: !793)
!800 = !DILocation(line: 352, column: 4, scope: !801)
!801 = distinct !DILexicalBlock(scope: !793, file: !3, line: 351, column: 2)
!802 = !DILocation(line: 353, column: 12, scope: !801)
!803 = !DILocation(line: 353, column: 18, scope: !801)
!804 = !DILocation(line: 353, column: 4, scope: !801)
!805 = distinct !{!805, !797, !806, !807}
!806 = !DILocation(line: 354, column: 2, scope: !793)
!807 = !{!"llvm.loop.mustprogress"}
!808 = !DILocation(line: 355, column: 16, scope: !793)
!809 = !DILocation(line: 355, column: 21, scope: !793)
!810 = !DILocation(line: 355, column: 15, scope: !793)
!811 = !DILocation(line: 355, column: 12, scope: !793)
!812 = !DILocation(line: 356, column: 5, scope: !793)
!813 = !DILocation(line: 0, scope: !497, inlinedAt: !814)
!814 = distinct !DILocation(line: 357, column: 10, scope: !620)
!815 = !DILocation(line: 190, column: 61, scope: !497, inlinedAt: !814)
!816 = !DILocation(line: 196, column: 18, scope: !497, inlinedAt: !814)
!817 = !DILocation(line: 197, column: 36, scope: !497, inlinedAt: !814)
!818 = !DILocation(line: 197, column: 22, scope: !497, inlinedAt: !814)
!819 = !DILocation(line: 199, column: 8, scope: !510, inlinedAt: !814)
!820 = !DILocation(line: 199, column: 7, scope: !497, inlinedAt: !814)
!821 = !DILocation(line: 201, column: 24, scope: !513, inlinedAt: !814)
!822 = !DILocation(line: 201, column: 11, scope: !514, inlinedAt: !814)
!823 = !DILocation(line: 204, column: 2, scope: !517, inlinedAt: !814)
!824 = !DILocation(line: 208, column: 8, scope: !519, inlinedAt: !814)
!825 = !DILocation(line: 208, column: 8, scope: !520, inlinedAt: !814)
!826 = !DILocation(line: 210, column: 25, scope: !523, inlinedAt: !814)
!827 = !DILocation(line: 210, column: 12, scope: !524, inlinedAt: !814)
!828 = !DILocation(line: 211, column: 3, scope: !523, inlinedAt: !814)
!829 = !DILocation(line: 0, scope: !520, inlinedAt: !814)
!830 = !DILocation(line: 212, column: 6, scope: !524, inlinedAt: !814)
!831 = !DILocation(line: 215, column: 25, scope: !530, inlinedAt: !814)
!832 = !DILocation(line: 215, column: 12, scope: !531, inlinedAt: !814)
!833 = !DILocation(line: 216, column: 3, scope: !530, inlinedAt: !814)
!834 = !DILocation(line: 0, scope: !519, inlinedAt: !814)
!835 = !DILocation(line: 0, scope: !513, inlinedAt: !814)
!836 = !DILocation(line: 219, column: 5, scope: !514, inlinedAt: !814)
!837 = !DILocation(line: 220, column: 15, scope: !497, inlinedAt: !814)
!838 = !DILocation(line: 220, column: 20, scope: !497, inlinedAt: !814)
!839 = !DILocation(line: 221, column: 25, scope: !541, inlinedAt: !814)
!840 = !DILocation(line: 221, column: 16, scope: !541, inlinedAt: !814)
!841 = !DILocation(line: 221, column: 13, scope: !541, inlinedAt: !814)
!842 = !DILocation(line: 221, column: 7, scope: !497, inlinedAt: !814)
!843 = !DILocation(line: 223, column: 18, scope: !546, inlinedAt: !814)
!844 = !DILocation(line: 224, column: 4, scope: !546, inlinedAt: !814)
!845 = !DILocation(line: 224, column: 14, scope: !546, inlinedAt: !814)
!846 = !DILocation(line: 224, column: 24, scope: !546, inlinedAt: !814)
!847 = !DILocation(line: 224, column: 39, scope: !546, inlinedAt: !814)
!848 = !DILocation(line: 224, column: 46, scope: !546, inlinedAt: !814)
!849 = !DILocation(line: 224, column: 44, scope: !546, inlinedAt: !814)
!850 = !DILocation(line: 224, column: 62, scope: !546, inlinedAt: !814)
!851 = !DILocation(line: 223, column: 11, scope: !547, inlinedAt: !814)
!852 = !DILocation(line: 226, column: 4, scope: !557, inlinedAt: !814)
!853 = !DILocation(line: 0, scope: !483, inlinedAt: !854)
!854 = distinct !DILocation(line: 227, column: 11, scope: !557, inlinedAt: !814)
!855 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !854)
!856 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !854)
!857 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !854)
!858 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !854)
!859 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !854)
!860 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !854)
!861 = !DILocation(line: 227, column: 59, scope: !557, inlinedAt: !814)
!862 = !DILocation(line: 227, column: 43, scope: !557, inlinedAt: !814)
!863 = !DILocation(line: 227, column: 41, scope: !557, inlinedAt: !814)
!864 = !DILocation(line: 227, column: 4, scope: !557, inlinedAt: !814)
!865 = !DILocation(line: 229, column: 16, scope: !571, inlinedAt: !814)
!866 = !DILocation(line: 229, column: 11, scope: !547, inlinedAt: !814)
!867 = !DILocation(line: 234, column: 25, scope: !575, inlinedAt: !814)
!868 = !DILocation(line: 234, column: 31, scope: !575, inlinedAt: !814)
!869 = !DILocation(line: 234, column: 4, scope: !575, inlinedAt: !814)
!870 = !DILocation(line: 236, column: 16, scope: !575, inlinedAt: !814)
!871 = !DILocation(line: 236, column: 21, scope: !575, inlinedAt: !814)
!872 = !DILocation(line: 237, column: 8, scope: !581, inlinedAt: !814)
!873 = !DILocation(line: 237, column: 15, scope: !581, inlinedAt: !814)
!874 = !DILocation(line: 237, column: 18, scope: !581, inlinedAt: !814)
!875 = !DILocation(line: 237, column: 8, scope: !575, inlinedAt: !814)
!876 = !DILocation(line: 238, column: 6, scope: !581, inlinedAt: !814)
!877 = !DILocation(line: 239, column: 2, scope: !575, inlinedAt: !814)
!878 = !DILocation(line: 240, column: 5, scope: !547, inlinedAt: !814)
!879 = !DILocation(line: 241, column: 7, scope: !589, inlinedAt: !814)
!880 = !DILocation(line: 241, column: 7, scope: !497, inlinedAt: !814)
!881 = !DILocation(line: 242, column: 27, scope: !589, inlinedAt: !814)
!882 = !DILocation(line: 242, column: 5, scope: !589, inlinedAt: !814)
!883 = !DILocation(line: 243, column: 11, scope: !497, inlinedAt: !814)
!884 = !DILocation(line: 243, column: 18, scope: !497, inlinedAt: !814)
!885 = !DILocation(line: 243, column: 16, scope: !497, inlinedAt: !814)
!886 = !DILocation(line: 243, column: 34, scope: !497, inlinedAt: !814)
!887 = !DILocation(line: 243, column: 8, scope: !497, inlinedAt: !814)
!888 = !DILocation(line: 244, column: 25, scope: !497, inlinedAt: !814)
!889 = !DILocation(line: 244, column: 34, scope: !497, inlinedAt: !814)
!890 = !DILocation(line: 244, column: 13, scope: !497, inlinedAt: !814)
!891 = !DILocation(line: 244, column: 40, scope: !497, inlinedAt: !814)
!892 = !DILocation(line: 244, column: 11, scope: !497, inlinedAt: !814)
!893 = !DILocation(line: 244, column: 8, scope: !497, inlinedAt: !814)
!894 = !DILocation(line: 245, column: 7, scope: !605, inlinedAt: !814)
!895 = !DILocation(line: 245, column: 12, scope: !605, inlinedAt: !814)
!896 = !DILocation(line: 245, column: 7, scope: !497, inlinedAt: !814)
!897 = !DILocation(line: 246, column: 5, scope: !605, inlinedAt: !814)
!898 = !DILocation(line: 247, column: 36, scope: !497, inlinedAt: !814)
!899 = !DILocation(line: 0, scope: !483, inlinedAt: !900)
!900 = distinct !DILocation(line: 247, column: 10, scope: !497, inlinedAt: !814)
!901 = !DILocation(line: 159, column: 21, scope: !483, inlinedAt: !900)
!902 = !DILocation(line: 159, column: 28, scope: !483, inlinedAt: !900)
!903 = !DILocation(line: 159, column: 48, scope: !483, inlinedAt: !900)
!904 = !DILocation(line: 159, column: 54, scope: !483, inlinedAt: !900)
!905 = !DILocation(line: 159, column: 35, scope: !483, inlinedAt: !900)
!906 = !DILocation(line: 159, column: 61, scope: !483, inlinedAt: !900)
!907 = !DILocation(line: 247, column: 3, scope: !497, inlinedAt: !814)
!908 = !DILocation(line: 357, column: 3, scope: !620)
!909 = !DILocation(line: 359, column: 1, scope: !620)
!910 = distinct !DISubprogram(name: "propagateFloat64NaN", linkageName: "_ZL19propagateFloat64NaNyy", scope: !397, file: !397, line: 109, type: !911, scopeLine: 110, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!911 = !DISubroutineType(types: !912)
!912 = !{!408, !408, !408}
!913 = !DILocalVariable(name: "a", arg: 1, scope: !910, file: !397, line: 109, type: !408)
!914 = !DILocation(line: 0, scope: !910)
!915 = !DILocalVariable(name: "b", arg: 2, scope: !910, file: !397, line: 109, type: !408)
!916 = !DILocation(line: 113, column: 12, scope: !910)
!917 = !DILocalVariable(name: "aIsNaN", scope: !910, file: !397, line: 111, type: !407)
!918 = !DILocation(line: 114, column: 21, scope: !910)
!919 = !DILocalVariable(name: "aIsSignalingNaN", scope: !910, file: !397, line: 111, type: !407)
!920 = !DILocation(line: 115, column: 12, scope: !910)
!921 = !DILocalVariable(name: "bIsNaN", scope: !910, file: !397, line: 111, type: !407)
!922 = !DILocation(line: 116, column: 21, scope: !910)
!923 = !DILocalVariable(name: "bIsSignalingNaN", scope: !910, file: !397, line: 111, type: !407)
!924 = !DILocation(line: 117, column: 5, scope: !910)
!925 = !DILocation(line: 118, column: 5, scope: !910)
!926 = !DILocation(line: 119, column: 23, scope: !927)
!927 = distinct !DILexicalBlock(scope: !910, file: !397, line: 119, column: 7)
!928 = !DILocation(line: 119, column: 7, scope: !927)
!929 = !DILocation(line: 119, column: 7, scope: !910)
!930 = !DILocation(line: 120, column: 5, scope: !927)
!931 = !DILocation(line: 121, column: 10, scope: !910)
!932 = !DILocation(line: 121, column: 32, scope: !910)
!933 = !DILocation(line: 121, column: 54, scope: !910)
!934 = !DILocation(line: 121, column: 3, scope: !910)
!935 = distinct !DISubprogram(name: "estimateDiv128To64", linkageName: "_ZL18estimateDiv128To64yyy", scope: !21, file: !21, line: 157, type: !936, scopeLine: 158, flags: DIFlagPrototyped, spFlags: DISPFlagLocalToUnit | DISPFlagDefinition, unit: !2, retainedNodes: !4)
!936 = !DISubroutineType(types: !937)
!937 = !{!6, !6, !6, !6}
!938 = !DILocalVariable(name: "a0", arg: 1, scope: !935, file: !21, line: 157, type: !6)
!939 = !DILocation(line: 0, scope: !935)
!940 = !DILocalVariable(name: "a1", arg: 2, scope: !935, file: !21, line: 157, type: !6)
!941 = !DILocalVariable(name: "b", arg: 3, scope: !935, file: !21, line: 157, type: !6)
!942 = !DILocalVariable(name: "rem0", scope: !935, file: !21, line: 160, type: !6)
!943 = !DILocation(line: 160, column: 10, scope: !935)
!944 = !DILocalVariable(name: "rem1", scope: !935, file: !21, line: 160, type: !6)
!945 = !DILocation(line: 160, column: 16, scope: !935)
!946 = !DILocalVariable(name: "term0", scope: !935, file: !21, line: 160, type: !6)
!947 = !DILocation(line: 160, column: 22, scope: !935)
!948 = !DILocalVariable(name: "term1", scope: !935, file: !21, line: 160, type: !6)
!949 = !DILocation(line: 160, column: 29, scope: !935)
!950 = !DILocation(line: 163, column: 9, scope: !951)
!951 = distinct !DILexicalBlock(scope: !935, file: !21, line: 163, column: 7)
!952 = !DILocation(line: 163, column: 7, scope: !935)
!953 = !DILocation(line: 164, column: 5, scope: !951)
!954 = !DILocation(line: 165, column: 10, scope: !935)
!955 = !DILocalVariable(name: "b0", scope: !935, file: !21, line: 159, type: !6)
!956 = !DILocation(line: 166, column: 11, scope: !935)
!957 = !DILocation(line: 166, column: 17, scope: !935)
!958 = !DILocation(line: 166, column: 7, scope: !935)
!959 = !DILocation(line: 166, column: 59, scope: !935)
!960 = !DILocation(line: 166, column: 65, scope: !935)
!961 = !DILocalVariable(name: "z", scope: !935, file: !21, line: 161, type: !6)
!962 = !DILocation(line: 167, column: 3, scope: !935)
!963 = !DILocation(line: 168, column: 19, scope: !935)
!964 = !DILocation(line: 168, column: 26, scope: !935)
!965 = !DILocation(line: 168, column: 3, scope: !935)
!966 = !DILocation(line: 169, column: 3, scope: !935)
!967 = !DILocation(line: 169, column: 21, scope: !935)
!968 = !DILocation(line: 169, column: 27, scope: !935)
!969 = !DILocation(line: 171, column: 9, scope: !970)
!970 = distinct !DILexicalBlock(scope: !935, file: !21, line: 170, column: 5)
!971 = !DILocation(line: 172, column: 14, scope: !970)
!972 = !DILocalVariable(name: "b1", scope: !935, file: !21, line: 159, type: !6)
!973 = !DILocation(line: 173, column: 15, scope: !970)
!974 = !DILocation(line: 173, column: 21, scope: !970)
!975 = !DILocation(line: 173, column: 7, scope: !970)
!976 = distinct !{!976, !966, !977, !807}
!977 = !DILocation(line: 174, column: 5, scope: !935)
!978 = !DILocation(line: 175, column: 11, scope: !935)
!979 = !DILocation(line: 175, column: 16, scope: !935)
!980 = !DILocation(line: 175, column: 26, scope: !935)
!981 = !DILocation(line: 175, column: 31, scope: !935)
!982 = !DILocation(line: 175, column: 23, scope: !935)
!983 = !DILocation(line: 175, column: 8, scope: !935)
!984 = !DILocation(line: 176, column: 12, scope: !935)
!985 = !DILocation(line: 176, column: 21, scope: !935)
!986 = !DILocation(line: 176, column: 18, scope: !935)
!987 = !DILocation(line: 176, column: 8, scope: !935)
!988 = !DILocation(line: 176, column: 42, scope: !935)
!989 = !DILocation(line: 176, column: 47, scope: !935)
!990 = !DILocation(line: 176, column: 5, scope: !935)
!991 = !DILocation(line: 177, column: 3, scope: !935)
!992 = !DILocation(line: 179, column: 1, scope: !935)
!993 = !DILocalVariable(name: "a", arg: 1, scope: !20, file: !21, line: 187, type: !24)
!994 = !DILocation(line: 0, scope: !20)
!995 = !DILocalVariable(name: "shiftCount", scope: !20, file: !21, line: 207, type: !16)
!996 = !DILocation(line: 210, column: 9, scope: !997)
!997 = distinct !DILexicalBlock(scope: !20, file: !21, line: 210, column: 7)
!998 = !DILocation(line: 210, column: 7, scope: !20)
!999 = !DILocation(line: 212, column: 18, scope: !1000)
!1000 = distinct !DILexicalBlock(scope: !997, file: !21, line: 211, column: 5)
!1001 = !DILocation(line: 213, column: 9, scope: !1000)
!1002 = !DILocation(line: 214, column: 5, scope: !1000)
!1003 = !DILocation(line: 215, column: 9, scope: !1004)
!1004 = distinct !DILexicalBlock(scope: !20, file: !21, line: 215, column: 7)
!1005 = !DILocation(line: 215, column: 7, scope: !20)
!1006 = !DILocation(line: 217, column: 18, scope: !1007)
!1007 = distinct !DILexicalBlock(scope: !1004, file: !21, line: 216, column: 5)
!1008 = !DILocation(line: 218, column: 9, scope: !1007)
!1009 = !DILocation(line: 219, column: 5, scope: !1007)
!1010 = !DILocation(line: 220, column: 41, scope: !20)
!1011 = !DILocation(line: 220, column: 17, scope: !20)
!1012 = !DILocation(line: 220, column: 14, scope: !20)
!1013 = !DILocation(line: 221, column: 3, scope: !20)
