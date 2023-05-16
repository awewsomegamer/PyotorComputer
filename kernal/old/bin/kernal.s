; ModuleID = 'kernal.c'
source_filename = "kernal.c"
target datalayout = "e-m:e-p:16:8-p1:8:8-i16:8-i32:8-i64:8-f32:8-f64:8-a:8-Fi8-n8"
target triple = "mos-common"

@cursor_index = dso_local local_unnamed_addr global i16 0, align 2
@VECTOR_CONSTANTS = internal global [3 x i16] [i16 -21589, i16 -17734, i16 -1286], section ".vectors", align 2
@.str = private unnamed_addr constant [4 x i8] c"GUH\00", align 1
@guh = dso_local local_unnamed_addr global ptr @.str, align 1
@logo_bmp = dso_local constant [30 x i8] c"\FF\FF\FF\FF\FF\FF\F7\E3\C9\9C\FF\FF\FF\FF\FF\FF\FE\FC\F9\F3>\07\FF\FF\FF\7F?\9F\CF\E7", align 1
@llvm.compiler.used = appending global [1 x ptr] [ptr @VECTOR_CONSTANTS], section "llvm.metadata"

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none)
define dso_local void @draw_pixel(i16 noundef %0, i16 noundef %1, i8 noundef zeroext %2) local_unnamed_addr #0 {
  %4 = mul nsw i16 %1, 320
  %5 = add nsw i16 %4, %0
  store i16 %5, ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 0, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 %2, ptr inttoptr (i16 -17021 to ptr), align 1, !tbaa !9
  store i8 -64, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none)
define dso_local void @draw_sprite(i16 noundef %0, i16 noundef %1, i8 noundef zeroext %2, i8 noundef zeroext %3) local_unnamed_addr #0 {
  %5 = mul nsw i16 %1, 40
  %6 = add nsw i16 %5, %0
  store i16 %6, ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 1, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 %2, ptr inttoptr (i16 -17021 to ptr), align 1, !tbaa !9
  %7 = shl i8 %3, 4
  %8 = and i8 %7, 16
  %9 = or i8 %8, -64
  store i8 %9, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(readwrite, argmem: write, inaccessiblemem: none)
define dso_local void @putc(i8 noundef zeroext %0, i8 noundef zeroext %1) local_unnamed_addr #1 {
  %3 = load i16, ptr @cursor_index, align 2, !tbaa !5
  store i16 %3, ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 3, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 %0, ptr inttoptr (i16 -17021 to ptr), align 1, !tbaa !9
  %4 = shl i8 %1, 4
  %5 = and i8 %4, 16
  %6 = or i8 %5, -64
  store i8 %6, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  %7 = load i16, ptr @cursor_index, align 2, !tbaa !5
  %8 = add i16 %7, 1
  store i16 %8, ptr @cursor_index, align 2, !tbaa !5
  ret void
}

; Function Attrs: minsize nofree norecurse nosync nounwind optsize memory(readwrite, inaccessiblemem: none)
define dso_local void @puts(ptr nocapture noundef readonly %0, i8 noundef zeroext %1) local_unnamed_addr #2 {
  br label %3

3:                                                ; preds = %9, %2
  %4 = phi i16 [ 0, %2 ], [ %10, %9 ]
  %5 = getelementptr inbounds i8, ptr %0, i16 %4
  %6 = load i8, ptr %5, align 1, !tbaa !9
  %7 = icmp eq i8 %6, 0
  br i1 %7, label %8, label %9

8:                                                ; preds = %3
  ret void

9:                                                ; preds = %3
  tail call void @putc(i8 noundef zeroext %6, i8 noundef zeroext %1) #5
  %10 = add nuw nsw i16 %4, 1
  br label %3, !llvm.loop !10
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none)
define dso_local void @set_fg(i8 noundef zeroext %0) local_unnamed_addr #0 {
  store i8 %0, ptr inttoptr (i16 -17019 to ptr), align 1, !tbaa !9
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none)
define dso_local void @set_bg(i8 noundef zeroext %0) local_unnamed_addr #0 {
  store i8 %0, ptr inttoptr (i16 -17018 to ptr), align 2, !tbaa !9
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none)
define dso_local void @set_sprite_table(i16 noundef zeroext %0) local_unnamed_addr #0 {
  store i16 %0, ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 2, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 -64, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(none)
define dso_local void @NMI_HANDLER() local_unnamed_addr #3 {
  ret void
}

; Function Attrs: minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(none)
define dso_local void @IRQ_HANDLER() local_unnamed_addr #3 {
  ret void
}

; Function Attrs: minsize nofree norecurse noreturn nosync nounwind optsize memory(readwrite, inaccessiblemem: none)
define dso_local i16 @main() local_unnamed_addr #4 {
  store i8 -1, ptr inttoptr (i16 -17019 to ptr), align 1, !tbaa !9
  store i16 ptrtoint (ptr @logo_bmp to i16), ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 2, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 -64, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  br label %1

1:                                                ; preds = %12, %0
  %2 = phi i16 [ 0, %0 ], [ %18, %12 ]
  %3 = phi i16 [ 0, %0 ], [ %14, %12 ]
  %4 = phi i16 [ 0, %0 ], [ %19, %12 ]
  %5 = icmp eq i16 %4, 6
  br i1 %5, label %6, label %8

6:                                                ; preds = %1
  %7 = load ptr, ptr @guh, align 1, !tbaa !12
  tail call void @puts(ptr noundef %7, i8 noundef zeroext 1) #5
  br label %20

8:                                                ; preds = %1
  %9 = icmp eq i16 %2, 3
  br i1 %9, label %10, label %12

10:                                               ; preds = %8
  %11 = add nsw i16 %3, 1
  br label %12

12:                                               ; preds = %10, %8
  %13 = phi i16 [ 0, %10 ], [ %2, %8 ]
  %14 = phi i16 [ %11, %10 ], [ %3, %8 ]
  %15 = trunc i16 %4 to i8
  %16 = mul nsw i16 %14, 40
  %17 = add nsw i16 %16, %13
  store i16 %17, ptr inttoptr (i16 -17024 to ptr), align 128, !tbaa !5
  store i8 1, ptr inttoptr (i16 -17022 to ptr), align 2, !tbaa !9
  store i8 %15, ptr inttoptr (i16 -17021 to ptr), align 1, !tbaa !9
  store i8 -48, ptr inttoptr (i16 -17020 to ptr), align 4, !tbaa !9
  %18 = add nsw i16 %13, 1
  %19 = add nuw nsw i16 %4, 1
  br label %1, !llvm.loop !14

20:                                               ; preds = %20, %6
  br label %20
}

attributes #0 = { minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(write, inaccessiblemem: none) "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(readwrite, argmem: write, inaccessiblemem: none) "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #2 = { minsize nofree norecurse nosync nounwind optsize memory(readwrite, inaccessiblemem: none) "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #3 = { minsize mustprogress nofree norecurse nosync nounwind optsize willreturn memory(none) "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #4 = { minsize nofree norecurse noreturn nosync nounwind optsize memory(readwrite, inaccessiblemem: none) "frame-pointer"="all" "no-builtins" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #5 = { minsize nobuiltin optsize "no-builtins" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{i32 1, !"ThinLTO", i32 0}
!3 = !{i32 1, !"EnableSplitLTOUnit", i32 1}
!4 = !{!"clang version 17.0.0 (https://www.github.com/llvm-mos/llvm-mos.git b05db96bef682be2ead7d306bc430156912d2598)"}
!5 = !{!6, !6, i64 0}
!6 = !{!"short", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!7, !7, i64 0}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.mustprogress"}
!12 = !{!13, !13, i64 0}
!13 = !{!"any pointer", !7, i64 0}
!14 = distinct !{!14, !11}

^0 = module: (path: "", hash: (0, 0, 0, 0, 0))
^1 = gv: (name: "IRQ_HANDLER", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 1, funcFlags: (readNone: 1, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 1740564866417236917
^2 = gv: (name: "putc", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 12, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), refs: (^15)))) ; guid = 3076685031351943535
^3 = gv: (name: ".str", summaries: (variable: (module: ^0, flags: (linkage: private, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 0, constant: 1)))) ; guid = 5246658939060452225
^4 = gv: (name: "logo_bmp", summaries: (variable: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 0, constant: 1)))) ; guid = 5395598711295786144
^5 = gv: (name: "set_sprite_table", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 4, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 6884295631132754406
^6 = gv: (name: "draw_sprite", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 10, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 7653789088922510787
^7 = gv: (name: "puts", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 10, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), calls: ((callee: ^2, relbf: 7904))))) ; guid = 8979701042202144121
^8 = gv: (name: "VECTOR_CONSTANTS", summaries: (variable: (module: ^0, flags: (linkage: internal, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 1, constant: 0)))) ; guid = 9466905419658936037
^9 = gv: (name: "llvm.compiler.used", summaries: (variable: (module: ^0, flags: (linkage: appending, visibility: default, notEligibleToImport: 1, live: 1, dsoLocal: 0, canAutoHide: 0), varFlags: (readonly: 0, writeonly: 0, constant: 0), refs: (^8)))) ; guid = 9610627770985738006
^10 = gv: (name: "draw_pixel", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 7, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 9954364126125837531
^11 = gv: (name: "guh", summaries: (variable: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 1, constant: 0), refs: (^3)))) ; guid = 10151346905821883373
^12 = gv: (name: "NMI_HANDLER", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 1, funcFlags: (readNone: 1, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 10200816846410269685
^13 = gv: (name: "set_fg", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 2, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 13309277019340021296
^14 = gv: (name: "main", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 30, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0), calls: ((callee: ^7, relbf: 256)), refs: (^4, ^11)))) ; guid = 15822663052811949562
^15 = gv: (name: "cursor_index", summaries: (variable: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), varFlags: (readonly: 1, writeonly: 1, constant: 0)))) ; guid = 15935289725462678485
^16 = gv: (name: "set_bg", summaries: (function: (module: ^0, flags: (linkage: external, visibility: default, notEligibleToImport: 1, live: 0, dsoLocal: 1, canAutoHide: 0), insts: 2, funcFlags: (readNone: 0, readOnly: 0, noRecurse: 1, returnDoesNotAlias: 0, noInline: 0, alwaysInline: 0, noUnwind: 1, mayThrow: 0, hasUnknownCall: 0, mustBeUnreachable: 0)))) ; guid = 18299326429226167420
^17 = flags: 8
^18 = blockcount: 19
