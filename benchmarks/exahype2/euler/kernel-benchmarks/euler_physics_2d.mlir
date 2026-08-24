module {
  func.func @maxEigenvalue_bridge(%arg0: !llvm.ptr, %arg1: memref<?xi8>, %arg2: memref<?xi8>, %arg3: f64, %arg4: f64, %arg5: i32, %arg6: !llvm.ptr) attributes {llvm.linkage = #llvm.linkage<external>} {
    %cst = arith.constant 0.000000e+00 : f64
    %c1_i32 = arith.constant 1 : i32
    %cst_0 = arith.constant 5.000000e-01 : f64
    %cst_1 = arith.constant 1.000000e+00 : f64
    %2 = llvm.load %arg0 : !llvm.ptr -> f64
    %3 = math.absf %2 : f64
    %4 = arith.divf %cst_1, %3 : f64
    %5 = llvm.getelementptr %arg0[3] : (!llvm.ptr) -> !llvm.ptr, f64
    %6 = llvm.load %5 : !llvm.ptr -> f64
    %7 = arith.mulf %4, %cst_0 : f64
    %8 = llvm.getelementptr %arg0[1] : (!llvm.ptr) -> !llvm.ptr, f64
    %9 = llvm.load %8 : !llvm.ptr -> f64
    %10 = arith.mulf %9, %9 : f64
    %11 = llvm.getelementptr %arg0[2] : (!llvm.ptr) -> !llvm.ptr, f64
    %12 = llvm.load %11 : !llvm.ptr -> f64
    %13 = arith.mulf %12, %12 : f64
    %14 = arith.addf %10, %13 : f64
    %15 = arith.mulf %7, %14 : f64
    %16 = arith.subf %6, %15 : f64
    %17 = arith.mulf %16, %cst : f64
    %18 = math.absf %17 : f64
    %19 = arith.mulf %18, %4 : f64
    %20 = math.sqrt %19 : f64
    %21 = arith.addi %arg5, %c1_i32 : i32
    %22 = llvm.getelementptr %arg0[%21] : (!llvm.ptr, i32) -> !llvm.ptr, f64
    %23 = llvm.load %22 : !llvm.ptr -> f64
    %24 = arith.mulf %23, %4 : f64
    %25 = arith.subf %24, %20 : f64
    %26 = math.absf %25 : f64
    %27 = arith.addf %24, %20 : f64
    %28 = math.absf %27 : f64
    %29 = arith.cmpf ogt, %28, %26 : f64
    %30 = arith.select %29, %28, %26 : f64
    llvm.store %30, %arg6 : f64, !llvm.ptr
    return
  }
  func.func @flux_bridge(%arg0: !llvm.ptr, %arg1: memref<?xi8>, %arg2: memref<?xi8>, %arg3: f64, %arg4: f64, %arg5: i32, %arg6: !llvm.ptr) attributes {llvm.linkage = #llvm.linkage<external>} {
     %cst = arith.constant 0.000000e+00 : f64
    %c1_i32 = arith.constant 1 : i32
    %cst_0 = arith.constant 5.000000e-01 : f64
    %cst_1 = arith.constant 1.000000e+00 : f64
    %2 = llvm.load %arg0 : !llvm.ptr -> f64
    %3 = arith.divf %cst_1, %2 : f64
    %4 = llvm.getelementptr %arg0[3] : (!llvm.ptr) -> !llvm.ptr, f64
    %5 = llvm.load %4 : !llvm.ptr -> f64
    %6 = arith.mulf %3, %cst_0 : f64
    %7 = llvm.getelementptr %arg0[1] : (!llvm.ptr) -> !llvm.ptr, f64
    %8 = llvm.load %7 : !llvm.ptr -> f64
    %9 = arith.mulf %8, %8 : f64
    %10 = llvm.getelementptr %arg0[2] : (!llvm.ptr) -> !llvm.ptr, f64
    %11 = llvm.load %10 : !llvm.ptr -> f64
    %12 = arith.mulf %11, %11 : f64
    %13 = arith.addf %9, %12 : f64
    %14 = arith.mulf %6, %13 : f64
    %15 = arith.subf %5, %14 : f64
    %16 = arith.mulf %15, %cst : f64
    %17 = arith.addi %arg5, %c1_i32 : i32
    %18 = llvm.getelementptr %arg0[%17] : (!llvm.ptr, i32) -> !llvm.ptr, f64
    %19 = llvm.load %18 : !llvm.ptr -> f64
    llvm.store %19, %arg6: f64, !llvm.ptr
    %20 = llvm.load %18 : !llvm.ptr -> f64
    %21 = llvm.load %7 : !llvm.ptr -> f64
    %22 = arith.mulf %20, %21 : f64
    %23 = arith.mulf %22, %3 : f64
    %24 = llvm.getelementptr %arg6[1] : (!llvm.ptr) -> !llvm.ptr, f64
    llvm.store %23, %24 : f64, !llvm.ptr
    %25 = llvm.load %18 : !llvm.ptr -> f64
    %26 = llvm.load %10 : !llvm.ptr -> f64
    %27 = arith.mulf %25, %26 : f64
    %28 = arith.mulf %27, %3 : f64
    %29 = llvm.getelementptr %arg6[2] : (!llvm.ptr) -> !llvm.ptr, f64
    llvm.store %28, %29 : f64, !llvm.ptr
    %30 = llvm.load %18 : !llvm.ptr -> f64
    %31 = llvm.load %4 : !llvm.ptr -> f64
    %32 = arith.addf %31, %16 : f64
    %33 = arith.mulf %30, %32 : f64
    %34 = arith.mulf %33, %3 : f64
    %35 = llvm.getelementptr %arg6[3] : (!llvm.ptr) -> !llvm.ptr, f64
    llvm.store %34, %35 : f64, !llvm.ptr
    %36 = llvm.getelementptr %arg6[%17] : (!llvm.ptr, i32) -> !llvm.ptr, f64
    %37 = llvm.load %36 : !llvm.ptr -> f64
    %38 = arith.addf %37, %16 : f64
    llvm.store %38, %36 : f64, !llvm.ptr
    return
  }
  func.func @nonconservativeProduct_bridge(%arg0: memref<?xi8>, %arg1: memref<?xi8>, %arg2: memref<?xi8>, %arg3: memref<?xi8>, %arg4: f64, %arg5: f64, %arg6: i32, %arg7: memref<?xi8>) attributes {llvm.linkage = #llvm.linkage<external>} {
    return
  }
  func.func @sourceTerm_bridge(%arg0: memref<?xi8>, %arg1: memref<?xi8>, %arg2: memref<?xi8>, %arg3: f64, %arg4: f64, %arg5: memref<?xi8>) attributes {llvm.linkage = #llvm.linkage<external>} {
    return
  }
}

