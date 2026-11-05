  (func $f1 (type $t3) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (param $p9 i32) (param $p10 i32) (param $p11 i32) (param $p12 f32) (param $p13 i32) (param $p14 i32) (result i32)
    (local $l15 i32) (local $l16 i32) (local $l17 i32) (local $l18 i32) (local $l19 i32) (local $l20 i32) (local $l21 i32) (local $l22 i32) (local $l23 i32) (local $l24 i32) (local $l25 i32) (local $l26 i32) (local $l27 i32) (local $l28 i32) (local $l29 i32) (local $l30 i32) (local $l31 i32) (local $l32 i32) (local $l33 i32) (local $l34 i32) (local $l35 i32) (local $l36 i32) (local $l37 i32) (local $l38 i32) (local $l39 i32) (local $l40 f32) (local $l41 i32) (local $l42 i32) (local $l43 i32) (local $l44 f32) (local $l45 f32) (local $l46 f32) (local $l47 f32) (local $l48 f32) (local $l49 f32) (local $l50 f32) (local $l51 f32) (local $l52 f32) (local $l53 f32) (local $l54 f32) (local $l55 f32) (local $l56 f32) (local $l57 f32) (local $l58 f32) (local $l59 f32) (local $l60 f32) (local $l61 f32) (local $l62 f32) (local $l63 f32) (local $l64 f32) (local $l65 f32) (local $l66 i32) (local $l67 f32) (local $l68 f32) (local $l69 f32) (local $l70 i32) (local $l71 i32) (local $l72 i32) (local $l73 i32) (local $l74 i32) (local $l75 i32) (local $l76 i32) (local $l77 i32) (local $l78 i32) (local $l79 f32) (local $l80 i32) (local $l81 i32) (local $l82 i32) (local $l83 i32) (local $l84 i32) (local $l85 i32) (local $l86 i32) (local $l87 i32) (local $l88 i32) (local $l89 i32) (local $l90 i32) (local $l91 i32) (local $l92 i64) (local $l93 f32) (local $l94 f32) (local $l95 f32) (local $l96 f32) (local $l97 f32) (local $l98 f32) (local $l99 f32) (local $l100 f32)
    global.get $g0
    i32.const 10496
    i32.sub
    local.tee $l15
    global.set $g0
    i32.const 0
    local.set $l16
    local.get $l15
    i32.const 156
    i32.add
    i32.const 0
    i32.const 100
    call $f23
    drop
    block $B0
      local.get $p0
      local.get $p1
      i32.eq
      br_if $B0
      local.get $p0
      local.get $p1
      local.get $p2
      i32.const 2
      i32.shl
      call $f22
      drop
    end
    block $B1
      local.get $p13
      i32.const 2
      i32.and
      i32.eqz
      br_if $B1
      local.get $p4
      i32.const 7
      i32.add
      i32.const 3
      i32.shr_u
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l17
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l17
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      i32.const 0
      local.set $p4
      i32.const 0
      local.set $p1
      block $B2
        local.get $p2
        i32.eqz
        br_if $B2
        local.get $p0
        local.set $p1
        local.get $p2
        local.set $l18
        loop $L3
          local.get $l17
          local.get $p1
          i32.load
          i32.const 3
          i32.shr_u
          i32.add
          i32.const 0
          i32.store8
          local.get $p1
          i32.const 4
          i32.add
          local.set $p1
          local.get $l18
          i32.const -1
          i32.add
          local.tee $l18
          br_if $L3
        end
        i32.const 0
        local.set $p4
        local.get $p0
        local.set $p1
        local.get $p2
        local.set $l18
        loop $L4
          local.get $l17
          local.get $p1
          i32.load
          local.tee $l19
          i32.const 3
          i32.shr_u
          i32.add
          local.tee $l20
          local.get $l20
          i32.load8_u
          local.tee $l20
          i32.const 1
          local.get $l19
          i32.const 7
          i32.and
          local.tee $l19
          i32.shl
          i32.or
          i32.store8
          local.get $l20
          i32.const -1
          i32.xor
          local.get $l19
          i32.shr_u
          i32.const 1
          i32.and
          local.get $p4
          i32.add
          local.set $p4
          local.get $p1
          i32.const 4
          i32.add
          local.set $p1
          local.get $l18
          i32.const -1
          i32.add
          local.tee $l18
          br_if $L4
        end
        i32.const -1
        local.get $p4
        i32.const 2
        i32.shl
        local.get $p4
        i32.const 1073741823
        i32.gt_u
        select
        local.set $p1
      end
      local.get $p1
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l16
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l16
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $p4
      i32.const 2
      i32.shr_u
      local.get $p4
      i32.add
      local.set $l18
      i32.const 1
      local.set $l17
      loop $L5
        local.get $l17
        local.tee $p1
        i32.const 1
        i32.shl
        local.set $l17
        local.get $p1
        local.get $l18
        i32.lt_u
        br_if $L5
      end
      i32.const 0
      local.set $l21
      i32.const -1
      local.get $p1
      i32.const 2
      i32.shl
      local.tee $l18
      local.get $p1
      i32.const 1073741823
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l17
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $l19
      i32.const 2
      i32.shl
      i32.add
      local.get $l17
      i32.store
      local.get $l15
      local.get $l19
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $l17
      i32.const 255
      local.get $l18
      call $f23
      local.set $l22
      block $B6
        local.get $p2
        i32.eqz
        br_if $B6
        local.get $p1
        i32.const -1
        i32.add
        local.set $l23
        i32.const 0
        local.set $l24
        loop $L7
          block $B8
            block $B9
              local.get $l22
              local.get $p0
              local.get $l24
              i32.const 2
              i32.shl
              i32.add
              local.tee $l25
              i32.load
              local.tee $l19
              i32.const 1540483477
              i32.mul
              local.get $l23
              i32.and
              local.tee $l18
              i32.const 2
              i32.shl
              i32.add
              local.tee $l20
              i32.load
              local.tee $l17
              i32.const -1
              i32.eq
              br_if $B9
              i32.const 1
              local.set $p1
              loop $L10
                local.get $l16
                local.get $l17
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.get $l19
                i32.eq
                br_if $B8
                local.get $l18
                local.get $p1
                i32.add
                local.set $l17
                local.get $p1
                i32.const 1
                i32.add
                local.set $p1
                local.get $l22
                local.get $l17
                local.get $l23
                i32.and
                local.tee $l18
                i32.const 2
                i32.shl
                i32.add
                local.tee $l20
                i32.load
                local.tee $l17
                i32.const -1
                i32.ne
                br_if $L10
              end
            end
            local.get $l16
            local.get $l21
            i32.const 2
            i32.shl
            i32.add
            local.get $l19
            i32.store
            local.get $l20
            local.get $l21
            i32.store
            local.get $l21
            local.set $l17
            local.get $l21
            i32.const 1
            i32.add
            local.set $l21
          end
          local.get $l25
          local.get $l17
          i32.store
          local.get $l24
          i32.const 1
          i32.add
          local.tee $l24
          local.get $p2
          i32.ne
          br_if $L7
        end
      end
      local.get $l22
      i32.const 0
      i32.load offset=37596
      call_indirect $T0 (type $t1)
      local.get $l15
      local.get $l15
      i32.load offset=252
      i32.const -1
      i32.add
      i32.store offset=252
    end
    i32.const 0
    local.set $l26
    i32.const -1
    local.get $p4
    i32.const 1
    i32.add
    local.tee $p1
    i32.const 2
    i32.shl
    local.get $p1
    i32.const 1073741823
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l24
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l24
    i32.store
    local.get $l15
    local.get $l24
    i32.store offset=148
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    i32.const -1
    local.get $p2
    i32.const 3
    i32.shl
    local.get $p2
    i32.const 536870911
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l27
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l27
    i32.store
    local.get $l15
    local.get $l27
    i32.store offset=152
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l15
    i32.const 148
    i32.add
    local.get $p0
    local.get $p2
    local.get $p4
    i32.const 0
    call $f2
    i32.const -1
    local.get $p4
    i32.const 2
    i32.shl
    local.tee $p1
    local.get $p4
    i32.const 1073741823
    i32.gt_u
    select
    local.tee $l28
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l17
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l18
    i32.const 2
    i32.shl
    i32.add
    local.get $l17
    i32.store
    local.get $l15
    local.get $l18
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l28
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l21
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l18
    i32.const 2
    i32.shl
    i32.add
    local.get $l21
    i32.store
    local.get $l15
    local.get $l18
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l17
    local.get $l21
    local.get $p3
    local.get $p4
    local.get $p5
    local.get $l16
    local.get $l15
    i32.const 156
    i32.add
    call $f3
    local.get $p4
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l29
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l18
    i32.const 2
    i32.shl
    i32.add
    local.get $l29
    i32.store
    local.get $l15
    local.get $l18
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l28
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l18
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l19
    i32.const 2
    i32.shl
    i32.add
    local.get $l18
    i32.store
    local.get $l15
    local.get $l19
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l28
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l19
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l20
    i32.const 2
    i32.shl
    i32.add
    local.get $l19
    i32.store
    local.get $l15
    local.get $l20
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l18
    i32.const 255
    local.get $p1
    call $f23
    local.set $l30
    local.get $l19
    i32.const 255
    local.get $p1
    call $f23
    local.set $l31
    block $B11
      local.get $p4
      i32.eqz
      br_if $B11
      loop $L12
        block $B13
          local.get $l24
          local.get $l26
          local.tee $l19
          i32.const 1
          i32.add
          local.tee $l26
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.tee $l20
          local.get $l24
          local.get $l19
          i32.const 2
          i32.shl
          local.tee $p1
          i32.add
          i32.load
          local.tee $l18
          i32.eq
          br_if $B13
          local.get $l20
          local.get $l18
          i32.sub
          local.set $l25
          local.get $l27
          local.get $l18
          i32.const 3
          i32.shl
          i32.add
          local.set $l32
          local.get $l31
          local.get $p1
          i32.add
          local.set $l33
          local.get $l30
          local.get $p1
          i32.add
          local.set $l23
          i32.const 0
          local.set $l20
          loop $L14
            block $B15
              block $B16
                local.get $l32
                local.get $l20
                i32.const 3
                i32.shl
                i32.add
                i32.load
                local.tee $l22
                local.get $l19
                i32.ne
                br_if $B16
                local.get $l23
                local.get $l19
                i32.store
                local.get $l33
                local.get $l19
                i32.store
                br $B15
              end
              block $B17
                local.get $l24
                local.get $l22
                i32.const 2
                i32.shl
                local.tee $l34
                i32.add
                local.tee $p1
                i32.const 4
                i32.add
                i32.load
                local.tee $l18
                local.get $p1
                i32.load
                local.tee $p1
                i32.eq
                br_if $B17
                local.get $l18
                local.get $p1
                i32.sub
                local.set $l18
                local.get $l27
                local.get $p1
                i32.const 3
                i32.shl
                i32.add
                local.set $p1
                loop $L18
                  local.get $p1
                  i32.load
                  local.get $l19
                  i32.eq
                  br_if $B15
                  local.get $p1
                  i32.const 8
                  i32.add
                  local.set $p1
                  local.get $l18
                  i32.const -1
                  i32.add
                  local.tee $l18
                  br_if $L18
                end
              end
              local.get $l31
              local.get $l34
              i32.add
              local.tee $p1
              local.get $l19
              local.get $l22
              local.get $p1
              i32.load
              i32.const -1
              i32.eq
              select
              i32.store
              local.get $l23
              local.get $l22
              local.get $l19
              local.get $l23
              i32.load
              i32.const -1
              i32.eq
              select
              i32.store
            end
            local.get $l20
            i32.const 1
            i32.add
            local.tee $l20
            local.get $l25
            i32.ne
            br_if $L14
          end
        end
        local.get $l26
        local.get $p4
        i32.ne
        br_if $L12
      end
      local.get $l17
      local.set $l18
      local.get $l21
      local.set $l19
      local.get $l30
      local.set $l22
      local.get $l31
      local.set $l20
      i32.const 0
      local.set $p1
      loop $L19
        block $B20
          block $B21
            local.get $p1
            local.get $l18
            i32.load
            local.tee $l23
            i32.ne
            br_if $B21
            block $B22
              local.get $p1
              local.get $l19
              i32.load
              local.tee $l23
              i32.ne
              br_if $B22
              local.get $l22
              i32.load
              local.set $l23
              block $B23
                local.get $l20
                i32.load
                local.tee $l25
                i32.const -1
                i32.ne
                br_if $B23
                local.get $l23
                i32.const -1
                i32.ne
                br_if $B23
                local.get $l29
                local.get $p1
                i32.add
                i32.const 0
                i32.store8
                br $B20
              end
              block $B24
                local.get $l25
                i32.const -1
                i32.eq
                br_if $B24
                local.get $l23
                i32.const -1
                i32.eq
                br_if $B24
                local.get $p1
                local.get $l25
                i32.eq
                br_if $B24
                local.get $l17
                local.get $l25
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.get $l17
                local.get $l23
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.ne
                br_if $B24
                local.get $l29
                local.get $p1
                i32.add
                i32.const 2
                i32.store8
                br $B20
              end
              local.get $l29
              local.get $p1
              i32.add
              local.set $l32
              block $B25
                local.get $p1
                local.get $l25
                i32.eq
                br_if $B25
                local.get $p1
                local.get $l23
                i32.eq
                br_if $B25
                local.get $l32
                i32.const 1
                i32.store8
                br $B20
              end
              local.get $l32
              i32.const 5
              i32.store8
              br $B20
            end
            block $B26
              local.get $p1
              local.get $l21
              local.get $l23
              i32.const 2
              i32.shl
              local.tee $l25
              i32.add
              i32.load
              i32.ne
              br_if $B26
              block $B27
                local.get $l20
                i32.load
                local.tee $l32
                i32.const -1
                i32.eq
                br_if $B27
                local.get $p1
                local.get $l32
                i32.eq
                br_if $B27
                local.get $l22
                i32.load
                local.tee $l34
                i32.const -1
                i32.eq
                br_if $B27
                local.get $p1
                local.get $l34
                i32.eq
                br_if $B27
                local.get $l31
                local.get $l25
                i32.add
                i32.load
                local.tee $l33
                i32.const -1
                i32.eq
                br_if $B27
                local.get $l33
                local.get $l23
                i32.eq
                br_if $B27
                local.get $l30
                local.get $l25
                i32.add
                i32.load
                local.tee $l25
                i32.const -1
                i32.eq
                br_if $B27
                local.get $l25
                local.get $l23
                i32.eq
                br_if $B27
                local.get $l17
                local.get $l32
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.tee $l23
                local.get $l17
                local.get $l25
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.ne
                br_if $B27
                local.get $l23
                local.get $l17
                local.get $l34
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.tee $l25
                i32.eq
                br_if $B27
                local.get $l25
                local.get $l17
                local.get $l33
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.ne
                br_if $B27
                local.get $l29
                local.get $p1
                i32.add
                i32.const 2
                i32.store8
                br $B20
              end
              local.get $l29
              local.get $p1
              i32.add
              i32.const 5
              i32.store8
              br $B20
            end
            local.get $l29
            local.get $p1
            i32.add
            i32.const 5
            i32.store8
            br $B20
          end
          local.get $l29
          local.get $p1
          i32.add
          local.get $l29
          local.get $l23
          i32.add
          i32.load8_u
          i32.store8
        end
        local.get $l18
        i32.const 4
        i32.add
        local.set $l18
        local.get $l19
        i32.const 4
        i32.add
        local.set $l19
        local.get $l22
        i32.const 4
        i32.add
        local.set $l22
        local.get $l20
        i32.const 4
        i32.add
        local.set $l20
        local.get $p4
        local.get $p1
        i32.const 1
        i32.add
        local.tee $p1
        i32.ne
        br_if $L19
      end
      block $B28
        local.get $p13
        i32.const 32
        i32.and
        i32.eqz
        br_if $B28
        i32.const 0
        local.set $l35
        loop $L29
          block $B30
            block $B31
              local.get $l29
              local.get $l35
              i32.add
              local.tee $l36
              i32.load8_u
              local.tee $l37
              i32.const -2
              i32.add
              br_table $B31 $B30 $B30 $B31 $B30
            end
            block $B32
              block $B33
                block $B34
                  local.get $l17
                  local.get $l35
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.tee $p1
                  local.get $l35
                  i32.ne
                  br_if $B34
                  i32.const 0
                  local.set $l25
                  i32.const 0
                  local.set $l38
                  local.get $p10
                  i32.eqz
                  br_if $B33
                  block $B35
                    local.get $l16
                    i32.eqz
                    br_if $B35
                    i32.const 0
                    local.set $l38
                    local.get $l35
                    local.set $p1
                    loop $L36
                      local.get $p10
                      local.get $l16
                      local.get $p1
                      i32.const 2
                      i32.shl
                      local.tee $p1
                      i32.add
                      i32.load
                      i32.add
                      i32.load8_u
                      i32.const 1
                      i32.shr_u
                      local.get $l38
                      i32.or
                      i32.const 1
                      i32.and
                      local.set $l38
                      local.get $l21
                      local.get $p1
                      i32.add
                      i32.load
                      local.tee $p1
                      local.get $l35
                      i32.ne
                      br_if $L36
                      br $B33
                    end
                  end
                  i32.const 0
                  local.set $l38
                  local.get $l35
                  local.set $p1
                  loop $L37
                    local.get $p10
                    local.get $p1
                    i32.add
                    i32.load8_u
                    i32.const 1
                    i32.shr_u
                    local.get $l38
                    i32.or
                    i32.const 1
                    i32.and
                    local.set $l38
                    local.get $l21
                    local.get $p1
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.tee $p1
                    local.get $l35
                    i32.ne
                    br_if $L37
                    br $B33
                  end
                end
                local.get $l29
                local.get $p1
                i32.add
                i32.load8_u
                local.set $p1
                br $B32
              end
              local.get $l35
              local.set $p1
              loop $L38
                block $B39
                  local.get $l24
                  local.get $p1
                  i32.const 2
                  i32.shl
                  local.tee $l26
                  i32.add
                  local.tee $p1
                  i32.const 4
                  i32.add
                  i32.load
                  local.tee $l18
                  local.get $p1
                  i32.load
                  local.tee $p1
                  i32.eq
                  br_if $B39
                  local.get $l18
                  local.get $p1
                  i32.sub
                  local.set $l34
                  local.get $l27
                  local.get $p1
                  i32.const 3
                  i32.shl
                  i32.add
                  local.set $l33
                  local.get $l17
                  local.get $l26
                  i32.add
                  local.set $l32
                  i32.const 0
                  local.set $l23
                  loop $L40
                    local.get $l33
                    local.get $l23
                    i32.const 3
                    i32.shl
                    i32.add
                    i32.load
                    local.tee $l22
                    local.set $p1
                    block $B41
                      loop $L42
                        block $B43
                          local.get $l24
                          local.get $p1
                          i32.const 2
                          i32.shl
                          local.tee $l20
                          i32.add
                          local.tee $p1
                          i32.const 4
                          i32.add
                          i32.load
                          local.tee $l18
                          local.get $p1
                          i32.load
                          local.tee $p1
                          i32.eq
                          br_if $B43
                          local.get $l18
                          local.get $p1
                          i32.sub
                          local.set $l18
                          local.get $l27
                          local.get $p1
                          i32.const 3
                          i32.shl
                          i32.add
                          local.set $p1
                          local.get $l32
                          i32.load
                          local.set $l19
                          block $B44
                            loop $L45
                              local.get $l17
                              local.get $p1
                              i32.load
                              i32.const 2
                              i32.shl
                              i32.add
                              i32.load
                              local.get $l19
                              i32.eq
                              br_if $B44
                              local.get $p1
                              i32.const 8
                              i32.add
                              local.set $p1
                              local.get $l18
                              i32.const -1
                              i32.add
                              local.tee $l18
                              i32.eqz
                              br_if $B43
                              br $L45
                            end
                          end
                          i32.const 0
                          local.set $p1
                          br $B41
                        end
                        local.get $l21
                        local.get $l20
                        i32.add
                        i32.load
                        local.tee $p1
                        local.get $l22
                        i32.ne
                        br_if $L42
                      end
                      i32.const 1
                      local.set $p1
                    end
                    local.get $l25
                    local.get $p1
                    i32.or
                    local.set $l25
                    local.get $l23
                    i32.const 1
                    i32.add
                    local.tee $l23
                    local.get $l34
                    i32.ne
                    br_if $L40
                  end
                end
                local.get $l21
                local.get $l26
                i32.add
                i32.load
                local.tee $p1
                local.get $l35
                i32.ne
                br_if $L38
              end
              local.get $l37
              i32.const 4
              i32.const 3
              local.get $l25
              i32.const 1
              i32.and
              select
              local.get $l38
              select
              local.set $p1
            end
            local.get $l36
            local.get $p1
            i32.store8
          end
          local.get $l35
          i32.const 1
          i32.add
          local.tee $l35
          local.get $p4
          i32.ne
          br_if $L29
        end
      end
      block $B46
        local.get $p10
        i32.eqz
        br_if $B46
        block $B47
          block $B48
            local.get $l16
            i32.eqz
            br_if $B48
            local.get $l16
            local.set $p1
            local.get $l17
            local.set $l18
            local.get $p4
            local.set $l19
            loop $L49
              block $B50
                local.get $p10
                local.get $p1
                i32.load
                i32.add
                i32.load8_u
                i32.const 1
                i32.and
                i32.eqz
                br_if $B50
                local.get $l29
                local.get $l18
                i32.load
                i32.add
                i32.const 5
                i32.store8
              end
              local.get $p1
              i32.const 4
              i32.add
              local.set $p1
              local.get $l18
              i32.const 4
              i32.add
              local.set $l18
              local.get $l19
              i32.const -1
              i32.add
              local.tee $l19
              br_if $L49
              br $B47
            end
          end
          local.get $p10
          local.set $p1
          local.get $l17
          local.set $l18
          local.get $p4
          local.set $l19
          loop $L51
            block $B52
              local.get $p1
              i32.load8_u
              i32.const 1
              i32.and
              i32.eqz
              br_if $B52
              local.get $l29
              local.get $l18
              i32.load
              i32.add
              i32.const 5
              i32.store8
            end
            local.get $p1
            i32.const 1
            i32.add
            local.set $p1
            local.get $l18
            i32.const 4
            i32.add
            local.set $l18
            local.get $l19
            i32.const -1
            i32.add
            local.tee $l19
            br_if $L51
          end
        end
        local.get $l17
        local.set $p1
        local.get $p4
        local.set $l19
        local.get $l29
        local.set $l18
        loop $L53
          block $B54
            local.get $l29
            local.get $p1
            i32.load
            i32.add
            i32.load8_u
            i32.const 5
            i32.ne
            br_if $B54
            local.get $l18
            i32.const 5
            i32.store8
          end
          local.get $p1
          i32.const 4
          i32.add
          local.set $p1
          local.get $l18
          i32.const 1
          i32.add
          local.set $l18
          local.get $l19
          i32.const -1
          i32.add
          local.tee $l19
          br_if $L53
        end
      end
      local.get $p13
      i32.const 1
      i32.and
      i32.eqz
      br_if $B11
      local.get $l29
      local.set $p1
      local.get $p4
      local.set $l18
      loop $L55
        block $B56
          block $B57
            local.get $p1
            i32.load8_u
            i32.const -1
            i32.add
            br_table $B57 $B56 $B56 $B57 $B56
          end
          local.get $p1
          i32.const 5
          i32.store8
        end
        local.get $p1
        i32.const 1
        i32.add
        local.set $p1
        local.get $l18
        i32.const -1
        i32.add
        local.tee $l18
        br_if $L55
      end
    end
    i32.const 0
    local.set $l39
    i32.const -1
    local.get $p4
    i32.const 12
    i32.mul
    local.get $p4
    i32.const 357913941
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l36
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l36
    i32.store
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l15
    i32.const 0
    i32.store offset=144
    local.get $l15
    i64.const 0
    i64.store offset=136
    local.get $l36
    local.get $p3
    local.get $p4
    local.get $p5
    local.get $l16
    local.get $l15
    i32.const 136
    i32.add
    call $f4
    local.set $l40
    block $B58
      block $B59
        local.get $p9
        br_if $B59
        i32.const 0
        local.set $l41
        i32.const 0
        local.set $l32
        br $B58
      end
      i32.const 0
      local.set $l32
      local.get $p8
      local.set $p1
      i32.const 0
      local.set $l18
      loop $L60
        block $B61
          local.get $p1
          f32.load
          f32.const 0x0p+0 (;=0;)
          f32.gt
          i32.eqz
          br_if $B61
          local.get $l15
          local.get $l32
          i32.const 2
          i32.shl
          i32.add
          local.get $l18
          i32.store
          local.get $l32
          i32.const 1
          i32.add
          local.set $l32
        end
        local.get $p1
        i32.const 4
        i32.add
        local.set $p1
        local.get $p9
        local.get $l18
        i32.const 1
        i32.add
        local.tee $l18
        i32.ne
        br_if $L60
      end
      i32.const -1
      local.get $l32
      local.get $p4
      i32.mul
      local.tee $p1
      i32.const 2
      i32.shl
      local.get $p1
      i32.const 1073741823
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l41
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l41
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $p4
      i32.eqz
      br_if $B58
      block $B62
        local.get $l32
        br_if $B62
        i32.const 0
        local.set $l32
        br $B58
      end
      local.get $p7
      i32.const 2
      i32.shr_u
      local.set $l34
      block $B63
        local.get $l16
        i32.eqz
        br_if $B63
        local.get $l32
        i32.const 2
        i32.shl
        local.set $l33
        i32.const 0
        local.set $l23
        local.get $l41
        local.set $l25
        loop $L64
          local.get $p6
          local.get $l16
          local.get $l23
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l34
          i32.mul
          i32.const 2
          i32.shl
          i32.add
          local.set $l22
          local.get $l15
          local.set $p1
          local.get $l25
          local.set $l18
          local.get $l32
          local.set $l19
          loop $L65
            local.get $l18
            local.get $l22
            local.get $p1
            i32.load
            i32.const 2
            i32.shl
            local.tee $l20
            i32.add
            f32.load
            local.get $p8
            local.get $l20
            i32.add
            f32.load
            f32.mul
            f32.store
            local.get $p1
            i32.const 4
            i32.add
            local.set $p1
            local.get $l18
            i32.const 4
            i32.add
            local.set $l18
            local.get $l19
            i32.const -1
            i32.add
            local.tee $l19
            br_if $L65
          end
          local.get $l25
          local.get $l33
          i32.add
          local.set $l25
          local.get $l23
          i32.const 1
          i32.add
          local.tee $l23
          local.get $p4
          i32.ne
          br_if $L64
          br $B58
        end
      end
      local.get $l32
      i32.const 2
      i32.shl
      local.set $l33
      i32.const 0
      local.set $l23
      local.get $l41
      local.set $l25
      loop $L66
        local.get $p6
        local.get $l23
        local.get $l34
        i32.mul
        i32.const 2
        i32.shl
        i32.add
        local.set $l22
        local.get $l15
        local.set $p1
        local.get $l25
        local.set $l18
        local.get $l32
        local.set $l19
        loop $L67
          local.get $l18
          local.get $l22
          local.get $p1
          i32.load
          i32.const 2
          i32.shl
          local.tee $l20
          i32.add
          f32.load
          local.get $p8
          local.get $l20
          i32.add
          f32.load
          f32.mul
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.set $p1
          local.get $l18
          i32.const 4
          i32.add
          local.set $l18
          local.get $l19
          i32.const -1
          i32.add
          local.tee $l19
          br_if $L67
        end
        local.get $l25
        local.get $l33
        i32.add
        local.set $l25
        local.get $l23
        i32.const 1
        i32.add
        local.tee $l23
        local.get $p4
        i32.ne
        br_if $L66
      end
    end
    i32.const -1
    local.get $p4
    i32.const 44
    i32.mul
    local.tee $l18
    local.get $p4
    i32.const 97612893
    i32.gt_u
    select
    local.tee $l20
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $p1
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $l19
    i32.const 2
    i32.shl
    i32.add
    local.get $p1
    i32.store
    local.get $l15
    local.get $l19
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $p1
    i32.const 0
    local.get $l18
    call $f23
    local.set $l42
    i32.const 0
    local.set $l43
    i32.const 0
    local.set $l34
    block $B68
      local.get $l32
      i32.eqz
      br_if $B68
      i32.const 0
      local.set $l39
      local.get $l20
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l34
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l34
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $l34
      i32.const 0
      local.get $l18
      call $f23
      drop
      i32.const -1
      local.get $l32
      local.get $p4
      i32.mul
      local.tee $p1
      i32.const 4
      i32.shl
      local.tee $l18
      local.get $p1
      i32.const 268435455
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l43
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l43
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $l43
      i32.const 0
      local.get $l18
      call $f23
      drop
      local.get $p13
      i32.const 536870912
      i32.and
      i32.eqz
      br_if $B68
      i32.const -1
      local.get $p4
      i32.const 4
      i32.shl
      local.tee $p1
      local.get $p4
      i32.const 268435455
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l39
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $l18
      i32.const 2
      i32.shl
      i32.add
      local.get $l39
      i32.store
      local.get $l15
      local.get $l18
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $l39
      i32.const 0
      local.get $p1
      call $f23
      drop
    end
    block $B69
      local.get $p2
      i32.eqz
      br_if $B69
      i32.const 0
      local.set $l22
      local.get $p0
      local.set $l18
      loop $L70
        local.get $l36
        local.get $l18
        i32.const 4
        i32.add
        i32.load
        local.tee $l23
        i32.const 12
        i32.mul
        i32.add
        local.tee $p1
        f32.load
        local.get $l36
        local.get $l18
        i32.load
        local.tee $l25
        i32.const 12
        i32.mul
        i32.add
        local.tee $l19
        f32.load
        local.tee $l44
        f32.sub
        local.tee $l45
        local.get $l36
        local.get $l18
        i32.const 8
        i32.add
        i32.load
        local.tee $l33
        i32.const 12
        i32.mul
        i32.add
        local.tee $l20
        f32.load offset=4
        local.get $l19
        f32.load offset=4
        local.tee $l46
        f32.sub
        local.tee $l47
        f32.mul
        local.get $p1
        f32.load offset=4
        local.get $l46
        f32.sub
        local.tee $l48
        local.get $l20
        f32.load
        local.get $l44
        f32.sub
        local.tee $l49
        f32.mul
        f32.sub
        local.tee $l50
        local.get $l50
        f32.mul
        local.get $l48
        local.get $l20
        f32.load offset=8
        local.get $l19
        f32.load offset=8
        local.tee $l51
        f32.sub
        local.tee $l52
        f32.mul
        local.get $p1
        f32.load offset=8
        local.get $l51
        f32.sub
        local.tee $l48
        local.get $l47
        f32.mul
        f32.sub
        local.tee $l53
        local.get $l53
        f32.mul
        local.get $l48
        local.get $l49
        f32.mul
        local.get $l45
        local.get $l52
        f32.mul
        f32.sub
        local.tee $l52
        local.get $l52
        f32.mul
        f32.add
        f32.add
        local.tee $l47
        f32.sqrt
        local.set $l54
        local.get $l50
        local.set $l49
        local.get $l52
        local.set $l48
        local.get $l53
        local.set $l45
        block $B71
          local.get $l47
          f32.const 0x0p+0 (;=0;)
          f32.gt
          local.tee $l19
          i32.eqz
          br_if $B71
          local.get $l50
          local.get $l54
          f32.div
          local.set $l49
          local.get $l52
          local.get $l54
          f32.div
          local.set $l48
          local.get $l53
          local.get $l54
          f32.div
          local.set $l45
        end
        local.get $l42
        local.get $l17
        local.get $l25
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l20
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l45
        local.get $l54
        f32.sqrt
        local.tee $l47
        local.get $l45
        f32.mul
        f32.mul
        local.tee $l55
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l48
        local.get $l47
        local.get $l48
        f32.mul
        local.tee $l56
        f32.mul
        local.tee $l57
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l49
        local.get $l47
        local.get $l49
        f32.mul
        local.tee $l58
        f32.mul
        local.tee $l59
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l45
        local.get $l56
        f32.mul
        local.tee $l56
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l58
        local.get $l45
        f32.mul
        local.tee $l60
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l48
        local.get $l58
        f32.mul
        local.tee $l61
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l45
        local.get $l47
        local.get $l49
        local.get $l51
        f32.mul
        local.get $l45
        local.get $l44
        f32.mul
        local.get $l46
        local.get $l48
        f32.mul
        f32.add
        f32.add
        f32.neg
        local.tee $l62
        f32.mul
        local.tee $l58
        f32.mul
        local.tee $l45
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l48
        local.get $l58
        f32.mul
        local.tee $l48
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l49
        local.get $l58
        f32.mul
        local.tee $l49
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l58
        local.get $l62
        f32.mul
        local.tee $l58
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l47
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        local.get $l42
        local.get $l17
        local.get $l23
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l23
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l55
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l57
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l59
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l56
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l60
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l61
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l45
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l48
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l49
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l58
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l47
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        local.get $l42
        local.get $l17
        local.get $l33
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l25
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l55
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l57
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l59
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l56
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l60
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l61
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l45
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l48
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l49
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l58
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l47
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        block $B72
          local.get $l39
          i32.eqz
          br_if $B72
          block $B73
            local.get $l19
            i32.eqz
            br_if $B73
            local.get $l50
            local.get $l54
            f32.div
            local.set $l50
            local.get $l52
            local.get $l54
            f32.div
            local.set $l52
            local.get $l53
            local.get $l54
            f32.div
            local.set $l53
          end
          local.get $l39
          local.get $l20
          i32.const 4
          i32.shl
          i32.add
          local.tee $p1
          local.get $l54
          f32.const 0x1p-1 (;=0.5;)
          f32.mul
          local.tee $l45
          local.get $l52
          f32.mul
          local.tee $l47
          local.get $p1
          f32.load offset=4
          f32.add
          f32.store offset=4
          local.get $p1
          local.get $l45
          local.get $l50
          f32.mul
          local.tee $l48
          local.get $p1
          f32.load offset=8
          f32.add
          f32.store offset=8
          local.get $p1
          local.get $l45
          local.get $l53
          f32.mul
          local.tee $l49
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          local.get $l45
          local.get $l44
          f32.neg
          local.get $l53
          f32.mul
          local.get $l46
          local.get $l52
          f32.mul
          f32.sub
          local.get $l51
          local.get $l50
          f32.mul
          f32.sub
          f32.mul
          local.tee $l45
          local.get $p1
          f32.load offset=12
          f32.add
          f32.store offset=12
          local.get $l39
          local.get $l23
          i32.const 4
          i32.shl
          i32.add
          local.tee $p1
          local.get $l47
          local.get $p1
          f32.load offset=4
          f32.add
          f32.store offset=4
          local.get $p1
          local.get $l48
          local.get $p1
          f32.load offset=8
          f32.add
          f32.store offset=8
          local.get $p1
          local.get $l49
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          local.get $l45
          local.get $p1
          f32.load offset=12
          f32.add
          f32.store offset=12
          local.get $l39
          local.get $l25
          i32.const 4
          i32.shl
          i32.add
          local.tee $p1
          local.get $l47
          local.get $p1
          f32.load offset=4
          f32.add
          f32.store offset=4
          local.get $p1
          local.get $l48
          local.get $p1
          f32.load offset=8
          f32.add
          f32.store offset=8
          local.get $p1
          local.get $l49
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          local.get $l45
          local.get $p1
          f32.load offset=12
          f32.add
          f32.store offset=12
        end
        local.get $l18
        i32.const 12
        i32.add
        local.set $l18
        local.get $l22
        i32.const 3
        i32.add
        local.tee $l22
        local.get $p2
        i32.lt_u
        br_if $L70
      end
    end
    block $B74
      local.get $p4
      i32.eqz
      br_if $B74
      f32.const 0x1.47ae14p-7 (;=0.01;)
      f32.const 0x1.99999ap-4 (;=0.1;)
      f32.const 0x1.ad7f2ap-24 (;=1e-07;)
      local.get $p13
      i32.const 16
      i32.and
      select
      local.get $p13
      i32.const 64
      i32.and
      select
      local.set $l52
      i32.const 0
      local.set $l18
      local.get $l17
      local.set $l20
      local.get $l16
      local.set $l22
      local.get $l42
      local.set $p1
      local.get $l36
      local.set $l19
      loop $L75
        block $B76
          local.get $l18
          local.get $l20
          i32.load
          i32.ne
          br_if $B76
          local.get $l18
          local.set $l23
          block $B77
            local.get $l16
            i32.eqz
            br_if $B77
            local.get $l22
            i32.load
            local.set $l23
          end
          local.get $l52
          local.set $l45
          block $B78
            local.get $p10
            i32.eqz
            br_if $B78
            f32.const 0x1p+0 (;=1;)
            local.get $l52
            local.get $p10
            local.get $l23
            i32.add
            i32.load8_u
            i32.const 4
            i32.and
            select
            local.set $l45
          end
          local.get $p1
          i32.const 12
          i32.add
          local.tee $l23
          local.get $l23
          f32.load
          f32.const 0x0p+0 (;=0;)
          f32.add
          f32.store
          local.get $p1
          i32.const 16
          i32.add
          local.tee $l23
          local.get $l23
          f32.load
          f32.const 0x0p+0 (;=0;)
          f32.add
          f32.store
          local.get $p1
          i32.const 20
          i32.add
          local.tee $l23
          local.get $l23
          f32.load
          f32.const 0x0p+0 (;=0;)
          f32.add
          f32.store
          local.get $p1
          local.get $l45
          local.get $p1
          i32.const 40
          i32.add
          local.tee $l23
          f32.load
          local.tee $l50
          f32.mul
          local.tee $l45
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.tee $l25
          local.get $l45
          local.get $l25
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 8
          i32.add
          local.tee $l25
          local.get $l45
          local.get $l25
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 24
          i32.add
          local.tee $l25
          local.get $l25
          f32.load
          local.get $l19
          f32.load
          local.tee $l48
          local.get $l45
          f32.mul
          f32.sub
          f32.store
          local.get $l19
          i32.const 8
          i32.add
          f32.load
          local.set $l47
          local.get $p1
          i32.const 28
          i32.add
          local.tee $l25
          local.get $l25
          f32.load
          local.get $l45
          local.get $l19
          i32.const 4
          i32.add
          f32.load
          local.tee $l49
          f32.mul
          f32.sub
          f32.store
          local.get $p1
          i32.const 32
          i32.add
          local.tee $l25
          local.get $l25
          f32.load
          local.get $l45
          local.get $l47
          f32.mul
          f32.sub
          f32.store
          local.get $p1
          i32.const 36
          i32.add
          local.tee $l25
          f32.load
          local.set $l53
          local.get $l23
          local.get $l50
          local.get $l45
          f32.add
          f32.store
          local.get $l25
          local.get $l53
          local.get $l45
          local.get $l47
          local.get $l47
          f32.mul
          local.get $l48
          local.get $l48
          f32.mul
          local.get $l49
          local.get $l49
          f32.mul
          f32.add
          f32.add
          f32.mul
          f32.add
          f32.store
        end
        local.get $l20
        i32.const 4
        i32.add
        local.set $l20
        local.get $l22
        i32.const 4
        i32.add
        local.set $l22
        local.get $p1
        i32.const 44
        i32.add
        local.set $p1
        local.get $l19
        i32.const 12
        i32.add
        local.set $l19
        local.get $p4
        local.get $l18
        i32.const 1
        i32.add
        local.tee $l18
        i32.ne
        br_if $L75
      end
    end
    block $B79
      local.get $p2
      i32.eqz
      br_if $B79
      i32.const 0
      local.set $l26
      local.get $p0
      local.set $l23
      loop $L80
        local.get $p0
        local.get $l26
        i32.const 2
        i32.shl
        i32.add
        local.set $l25
        i32.const 0
        local.set $p1
        loop $L81
          block $B82
            local.get $l29
            local.get $l23
            local.get $p1
            i32.add
            i32.load
            local.tee $l20
            i32.add
            i32.load8_u
            local.tee $l18
            i32.eqz
            br_if $B82
            local.get $l29
            local.get $l25
            local.get $p1
            i32.const 36896
            i32.add
            i32.load
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $l22
            i32.add
            i32.load8_u
            i32.const 255
            i32.and
            local.tee $l19
            i32.eqz
            br_if $B82
            block $B83
              local.get $l18
              i32.const 4
              i32.eq
              local.tee $l33
              br_if $B83
              local.get $l18
              i32.const 3
              i32.lt_u
              br_if $B83
              local.get $l19
              i32.const 4
              i32.eq
              br_if $B83
              local.get $l19
              i32.const 2
              i32.gt_u
              br_if $B82
            end
            block $B84
              local.get $l18
              i32.const 2
              i32.gt_u
              br_if $B84
              local.get $l30
              local.get $l20
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.get $l22
              i32.ne
              br_if $B82
            end
            block $B85
              local.get $l19
              i32.const 2
              i32.gt_u
              br_if $B85
              local.get $l31
              local.get $l22
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.get $l20
              i32.ne
              br_if $B82
            end
            block $B86
              block $B87
                local.get $l33
                br_if $B87
                local.get $l19
                i32.const 4
                i32.ne
                br_if $B86
              end
              local.get $l18
              i32.const 3
              i32.eq
              br_if $B82
              local.get $l19
              i32.const 3
              i32.eq
              br_if $B82
            end
            f32.const 0x1p-1 (;=0.5;)
            f32.const 0x1p-1 (;=0.5;)
            f32.const 0x1.4p+3 (;=10;)
            local.get $l19
            i32.const 2
            i32.eq
            select
            local.get $l18
            i32.const 2
            i32.eq
            select
            local.set $l56
            block $B88
              local.get $l36
              local.get $l25
              local.get $p1
              i32.const 36900
              i32.add
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              i32.const 12
              i32.mul
              i32.add
              local.tee $l18
              f32.load offset=8
              local.get $l36
              local.get $l20
              i32.const 12
              i32.mul
              i32.add
              local.tee $l19
              f32.load offset=8
              local.tee $l58
              f32.sub
              local.tee $l44
              local.get $l36
              local.get $l22
              i32.const 12
              i32.mul
              i32.add
              local.tee $l33
              f32.load offset=8
              local.get $l58
              f32.sub
              local.tee $l49
              local.get $l49
              f32.mul
              local.get $l33
              f32.load
              local.get $l19
              f32.load
              local.tee $l46
              f32.sub
              local.tee $l52
              local.get $l52
              f32.mul
              local.get $l33
              f32.load offset=4
              local.get $l19
              f32.load offset=4
              local.tee $l51
              f32.sub
              local.tee $l47
              local.get $l47
              f32.mul
              f32.add
              f32.add
              local.tee $l53
              f32.mul
              local.get $l49
              local.get $l44
              local.get $l49
              f32.mul
              local.get $l18
              f32.load
              local.get $l46
              f32.sub
              local.tee $l55
              local.get $l52
              f32.mul
              local.get $l47
              local.get $l18
              f32.load offset=4
              local.get $l51
              f32.sub
              local.tee $l57
              f32.mul
              f32.add
              f32.add
              local.tee $l48
              f32.mul
              f32.sub
              local.tee $l50
              local.get $l50
              f32.mul
              local.get $l55
              local.get $l53
              f32.mul
              local.get $l52
              local.get $l48
              f32.mul
              f32.sub
              local.tee $l45
              local.get $l45
              f32.mul
              local.get $l57
              local.get $l53
              f32.mul
              local.get $l47
              local.get $l48
              f32.mul
              f32.sub
              local.tee $l48
              local.get $l48
              f32.mul
              f32.add
              f32.add
              local.tee $l54
              f32.const 0x0p+0 (;=0;)
              f32.gt
              i32.eqz
              br_if $B88
              local.get $l50
              local.get $l54
              f32.sqrt
              local.tee $l54
              f32.div
              local.set $l50
              local.get $l48
              local.get $l54
              f32.div
              local.set $l48
              local.get $l45
              local.get $l54
              f32.div
              local.set $l45
            end
            local.get $l56
            local.get $l53
            f32.sqrt
            f32.mul
            local.tee $l54
            local.get $l50
            local.get $l58
            f32.mul
            local.get $l45
            local.get $l46
            f32.mul
            local.get $l51
            local.get $l48
            f32.mul
            f32.add
            f32.add
            f32.neg
            local.tee $l53
            f32.mul
            local.tee $l59
            local.get $l53
            f32.mul
            local.set $l60
            local.get $l52
            local.get $l57
            f32.mul
            local.get $l47
            local.get $l55
            f32.mul
            f32.sub
            local.tee $l53
            local.get $l53
            f32.mul
            local.get $l47
            local.get $l44
            f32.mul
            local.get $l49
            local.get $l57
            f32.mul
            f32.sub
            local.tee $l47
            local.get $l47
            f32.mul
            local.get $l49
            local.get $l55
            f32.mul
            local.get $l52
            local.get $l44
            f32.mul
            f32.sub
            local.tee $l49
            local.get $l49
            f32.mul
            f32.add
            f32.add
            local.tee $l55
            f32.sqrt
            local.set $l52
            local.get $l50
            local.get $l59
            f32.mul
            local.set $l57
            local.get $l48
            local.get $l59
            f32.mul
            local.set $l61
            local.get $l45
            local.get $l59
            f32.mul
            local.set $l59
            local.get $l48
            local.get $l54
            local.get $l50
            f32.mul
            local.tee $l44
            f32.mul
            local.set $l62
            local.get $l44
            local.get $l45
            f32.mul
            local.set $l63
            local.get $l45
            local.get $l54
            local.get $l48
            f32.mul
            local.tee $l64
            f32.mul
            local.set $l65
            local.get $l50
            local.get $l44
            f32.mul
            local.set $l50
            local.get $l48
            local.get $l64
            f32.mul
            local.set $l48
            local.get $l45
            local.get $l54
            local.get $l45
            f32.mul
            f32.mul
            local.set $l44
            block $B89
              local.get $l55
              f32.const 0x0p+0 (;=0;)
              f32.gt
              i32.eqz
              br_if $B89
              local.get $l53
              local.get $l52
              f32.div
              local.set $l53
              local.get $l49
              local.get $l52
              f32.div
              local.set $l49
              local.get $l47
              local.get $l52
              f32.div
              local.set $l47
            end
            local.get $l42
            local.get $l17
            local.get $l20
            i32.const 2
            i32.shl
            i32.add
            i32.load
            i32.const 44
            i32.mul
            i32.add
            local.tee $l18
            local.get $l18
            f32.load
            local.get $l44
            local.get $l47
            local.get $l56
            local.get $l52
            f32.sqrt
            f32.mul
            local.tee $l45
            local.get $l47
            f32.mul
            f32.mul
            f32.add
            local.tee $l52
            f32.add
            f32.store
            local.get $l18
            local.get $l48
            local.get $l49
            local.get $l45
            local.get $l49
            f32.mul
            local.tee $l44
            f32.mul
            f32.add
            local.tee $l55
            local.get $l18
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $l18
            local.get $l50
            local.get $l53
            local.get $l45
            local.get $l53
            f32.mul
            local.tee $l48
            f32.mul
            f32.add
            local.tee $l50
            local.get $l18
            f32.load offset=8
            f32.add
            f32.store offset=8
            local.get $l18
            local.get $l65
            local.get $l47
            local.get $l44
            f32.mul
            f32.add
            local.tee $l44
            local.get $l18
            f32.load offset=12
            f32.add
            f32.store offset=12
            local.get $l18
            local.get $l63
            local.get $l48
            local.get $l47
            f32.mul
            f32.add
            local.tee $l56
            local.get $l18
            f32.load offset=16
            f32.add
            f32.store offset=16
            local.get $l18
            local.get $l62
            local.get $l49
            local.get $l48
            f32.mul
            f32.add
            local.tee $l48
            local.get $l18
            f32.load offset=20
            f32.add
            f32.store offset=20
            local.get $l18
            local.get $l59
            local.get $l47
            local.get $l45
            local.get $l53
            local.get $l58
            f32.mul
            local.get $l47
            local.get $l46
            f32.mul
            local.get $l51
            local.get $l49
            f32.mul
            f32.add
            f32.add
            local.tee $l58
            f32.neg
            f32.mul
            local.tee $l45
            f32.mul
            f32.add
            local.tee $l47
            local.get $l18
            f32.load offset=24
            f32.add
            f32.store offset=24
            local.get $l18
            local.get $l61
            local.get $l49
            local.get $l45
            f32.mul
            f32.add
            local.tee $l49
            local.get $l18
            f32.load offset=28
            f32.add
            f32.store offset=28
            local.get $l18
            local.get $l57
            local.get $l53
            local.get $l45
            f32.mul
            f32.add
            local.tee $l53
            local.get $l18
            f32.load offset=32
            f32.add
            f32.store offset=32
            local.get $l18
            local.get $l60
            local.get $l58
            local.get $l45
            f32.mul
            f32.sub
            local.tee $l45
            local.get $l18
            f32.load offset=36
            f32.add
            f32.store offset=36
            local.get $l18
            local.get $l54
            local.get $l18
            f32.load offset=40
            f32.add
            f32.store offset=40
            local.get $l42
            local.get $l17
            local.get $l22
            i32.const 2
            i32.shl
            i32.add
            i32.load
            i32.const 44
            i32.mul
            i32.add
            local.tee $l18
            local.get $l52
            local.get $l18
            f32.load
            f32.add
            f32.store
            local.get $l18
            local.get $l55
            local.get $l18
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $l18
            local.get $l50
            local.get $l18
            f32.load offset=8
            f32.add
            f32.store offset=8
            local.get $l18
            local.get $l44
            local.get $l18
            f32.load offset=12
            f32.add
            f32.store offset=12
            local.get $l18
            local.get $l56
            local.get $l18
            f32.load offset=16
            f32.add
            f32.store offset=16
            local.get $l18
            local.get $l48
            local.get $l18
            f32.load offset=20
            f32.add
            f32.store offset=20
            local.get $l18
            local.get $l47
            local.get $l18
            f32.load offset=24
            f32.add
            f32.store offset=24
            local.get $l18
            local.get $l49
            local.get $l18
            f32.load offset=28
            f32.add
            f32.store offset=28
            local.get $l18
            local.get $l53
            local.get $l18
            f32.load offset=32
            f32.add
            f32.store offset=32
            local.get $l18
            local.get $l45
            local.get $l18
            f32.load offset=36
            f32.add
            f32.store offset=36
            local.get $l18
            local.get $l54
            local.get $l18
            f32.load offset=40
            f32.add
            f32.store offset=40
          end
          local.get $p1
          i32.const 4
          i32.add
          local.tee $p1
          i32.const 12
          i32.ne
          br_if $L81
        end
        local.get $l23
        i32.const 12
        i32.add
        local.set $l23
        local.get $l26
        i32.const 3
        i32.add
        local.tee $l26
        local.get $p2
        i32.lt_u
        br_if $L80
      end
    end
    block $B90
      local.get $p13
      i32.const 128
      i32.and
      i32.eqz
      br_if $B90
      local.get $p4
      i32.eqz
      br_if $B90
      i32.const 0
      local.set $l66
      loop $L91
        block $B92
          local.get $l24
          local.get $l66
          local.tee $p1
          i32.const 2
          i32.shl
          local.tee $l18
          i32.add
          i32.load
          local.tee $l33
          local.get $l24
          local.get $p1
          i32.const 1
          i32.add
          local.tee $l66
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.tee $l38
          i32.ge_u
          br_if $B92
          local.get $l36
          local.get $p1
          i32.const 12
          i32.mul
          i32.add
          local.set $p9
          local.get $l42
          local.get $l17
          local.get $l18
          i32.add
          i32.load
          local.tee $l20
          i32.const 44
          i32.mul
          i32.add
          local.set $l26
          loop $L93
            block $B94
              local.get $l17
              local.get $l27
              local.get $l33
              i32.const 3
              i32.shl
              i32.add
              local.tee $p1
              i32.load
              local.tee $l23
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l35
              local.get $l20
              i32.gt_u
              br_if $B94
              local.get $p1
              i32.load offset=4
              local.set $l37
              i32.const -1
              local.set $l19
              local.get $l23
              local.set $p1
              loop $L95
                block $B96
                  local.get $l24
                  local.get $p1
                  i32.const 2
                  i32.shl
                  local.tee $l25
                  i32.add
                  local.tee $p1
                  i32.const 4
                  i32.add
                  i32.load
                  local.tee $l18
                  local.get $p1
                  i32.load
                  local.tee $p1
                  i32.eq
                  br_if $B96
                  local.get $l18
                  local.get $p1
                  i32.sub
                  local.set $l18
                  local.get $l27
                  local.get $p1
                  i32.const 3
                  i32.shl
                  i32.add
                  local.set $p1
                  loop $L97
                    block $B98
                      local.get $l17
                      local.get $p1
                      i32.load
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      local.get $l20
                      i32.ne
                      br_if $B98
                      local.get $l19
                      i32.const -1
                      i32.ne
                      local.set $l22
                      local.get $l23
                      local.set $l19
                      local.get $l22
                      br_if $B98
                      local.get $p1
                      i32.const 4
                      i32.add
                      i32.load
                      local.set $l19
                    end
                    local.get $p1
                    i32.const 8
                    i32.add
                    local.set $p1
                    local.get $l18
                    i32.const -1
                    i32.add
                    local.tee $l18
                    br_if $L97
                  end
                end
                local.get $l21
                local.get $l25
                i32.add
                i32.load
                local.tee $p1
                local.get $l23
                i32.ne
                br_if $L95
              end
              local.get $l19
              local.get $l23
              i32.eq
              br_if $B94
              local.get $l19
              i32.const -1
              i32.eq
              br_if $B94
              local.get $l36
              local.get $l23
              i32.const 12
              i32.mul
              i32.add
              local.tee $p1
              f32.load
              local.get $p9
              f32.load
              local.tee $l53
              f32.sub
              local.tee $l45
              local.get $l36
              local.get $l37
              i32.const 12
              i32.mul
              i32.add
              local.tee $l18
              f32.load offset=4
              local.get $p9
              f32.load offset=4
              local.tee $l52
              f32.sub
              local.tee $l49
              f32.mul
              local.get $p1
              f32.load offset=4
              local.get $l52
              f32.sub
              local.tee $l47
              local.get $l18
              f32.load
              local.get $l53
              f32.sub
              local.tee $l50
              f32.mul
              f32.sub
              local.tee $l58
              local.get $l36
              local.get $l19
              i32.const 12
              i32.mul
              i32.add
              local.tee $l19
              f32.load
              local.get $l53
              f32.sub
              local.tee $l51
              local.get $l47
              f32.mul
              local.get $l19
              f32.load offset=4
              local.get $l52
              f32.sub
              local.tee $l55
              local.get $l45
              f32.mul
              f32.sub
              local.tee $l44
              f32.mul
              local.get $l47
              local.get $l18
              f32.load offset=8
              local.get $p9
              f32.load offset=8
              local.tee $l54
              f32.sub
              local.tee $l46
              f32.mul
              local.get $p1
              f32.load offset=8
              local.get $l54
              f32.sub
              local.tee $l48
              local.get $l49
              f32.mul
              f32.sub
              local.tee $l57
              local.get $l55
              local.get $l48
              f32.mul
              local.get $l19
              f32.load offset=8
              local.get $l54
              f32.sub
              local.tee $l59
              local.get $l47
              f32.mul
              f32.sub
              local.tee $l56
              f32.mul
              local.get $l48
              local.get $l50
              f32.mul
              local.get $l45
              local.get $l46
              f32.mul
              f32.sub
              local.tee $l60
              local.get $l59
              local.get $l45
              f32.mul
              local.get $l51
              local.get $l48
              f32.mul
              f32.sub
              local.tee $l61
              f32.mul
              f32.add
              f32.add
              local.get $l58
              local.get $l58
              f32.mul
              local.get $l57
              local.get $l57
              f32.mul
              local.get $l60
              local.get $l60
              f32.mul
              f32.add
              f32.add
              f32.sqrt
              f32.const -0x1.b33334p-1 (;=-0.85;)
              f32.mul
              local.get $l44
              local.get $l44
              f32.mul
              local.get $l56
              local.get $l56
              f32.mul
              local.get $l61
              local.get $l61
              f32.mul
              f32.add
              f32.add
              f32.sqrt
              f32.mul
              f32.ge
              br_if $B94
              block $B99
                local.get $l46
                local.get $l48
                local.get $l48
                f32.mul
                local.get $l45
                local.get $l45
                f32.mul
                local.get $l47
                local.get $l47
                f32.mul
                f32.add
                f32.add
                local.tee $l44
                f32.mul
                local.get $l48
                local.get $l46
                local.get $l48
                f32.mul
                local.get $l50
                local.get $l45
                f32.mul
                local.get $l47
                local.get $l49
                f32.mul
                f32.add
                f32.add
                local.tee $l58
                f32.mul
                f32.sub
                local.tee $l46
                local.get $l46
                f32.mul
                local.get $l50
                local.get $l44
                f32.mul
                local.get $l45
                local.get $l58
                f32.mul
                f32.sub
                local.tee $l50
                local.get $l50
                f32.mul
                local.get $l49
                local.get $l44
                f32.mul
                local.get $l47
                local.get $l58
                f32.mul
                f32.sub
                local.tee $l58
                local.get $l58
                f32.mul
                f32.add
                f32.add
                local.tee $l49
                f32.const 0x0p+0 (;=0;)
                f32.gt
                i32.eqz
                br_if $B99
                local.get $l46
                local.get $l49
                f32.sqrt
                local.tee $l49
                f32.div
                local.set $l46
                local.get $l58
                local.get $l49
                f32.div
                local.set $l58
                local.get $l50
                local.get $l49
                f32.div
                local.set $l50
              end
              local.get $l44
              f32.sqrt
              local.tee $l49
              local.get $l46
              local.get $l54
              f32.mul
              local.get $l50
              local.get $l53
              f32.mul
              local.get $l52
              local.get $l58
              f32.mul
              f32.add
              f32.add
              f32.neg
              local.tee $l56
              f32.mul
              local.tee $l57
              local.get $l56
              f32.mul
              local.set $l56
              local.get $l46
              local.get $l57
              f32.mul
              local.set $l60
              local.get $l58
              local.get $l57
              f32.mul
              local.set $l61
              local.get $l50
              local.get $l57
              f32.mul
              local.set $l62
              local.get $l58
              local.get $l49
              local.get $l46
              f32.mul
              local.tee $l57
              f32.mul
              local.set $l63
              local.get $l57
              local.get $l50
              f32.mul
              local.set $l64
              local.get $l50
              local.get $l49
              local.get $l58
              f32.mul
              local.tee $l65
              f32.mul
              local.set $l67
              local.get $l46
              local.get $l57
              f32.mul
              local.set $l46
              local.get $l58
              local.get $l65
              f32.mul
              local.set $l58
              local.get $l50
              local.get $l49
              local.get $l50
              f32.mul
              f32.mul
              local.set $l57
              block $B100
                local.get $l59
                local.get $l44
                f32.mul
                local.get $l48
                local.get $l59
                local.get $l48
                f32.mul
                local.get $l51
                local.get $l45
                f32.mul
                local.get $l47
                local.get $l55
                f32.mul
                f32.add
                f32.add
                local.tee $l50
                f32.mul
                f32.sub
                local.tee $l48
                local.get $l48
                f32.mul
                local.get $l51
                local.get $l44
                f32.mul
                local.get $l45
                local.get $l50
                f32.mul
                f32.sub
                local.tee $l45
                local.get $l45
                f32.mul
                local.get $l55
                local.get $l44
                f32.mul
                local.get $l47
                local.get $l50
                f32.mul
                f32.sub
                local.tee $l47
                local.get $l47
                f32.mul
                f32.add
                f32.add
                local.tee $l50
                f32.const 0x0p+0 (;=0;)
                f32.gt
                i32.eqz
                br_if $B100
                local.get $l48
                local.get $l50
                f32.sqrt
                local.tee $l50
                f32.div
                local.set $l48
                local.get $l47
                local.get $l50
                f32.div
                local.set $l47
                local.get $l45
                local.get $l50
                f32.div
                local.set $l45
              end
              local.get $l26
              local.get $l26
              f32.load
              local.get $l57
              local.get $l45
              local.get $l49
              local.get $l45
              f32.mul
              f32.mul
              f32.add
              local.tee $l44
              f32.add
              f32.store
              local.get $l26
              local.get $l58
              local.get $l47
              local.get $l49
              local.get $l47
              f32.mul
              local.tee $l51
              f32.mul
              f32.add
              local.tee $l58
              local.get $l26
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $l26
              local.get $l46
              local.get $l48
              local.get $l49
              local.get $l48
              f32.mul
              local.tee $l50
              f32.mul
              f32.add
              local.tee $l46
              local.get $l26
              f32.load offset=8
              f32.add
              f32.store offset=8
              local.get $l26
              local.get $l67
              local.get $l45
              local.get $l51
              f32.mul
              f32.add
              local.tee $l51
              local.get $l26
              f32.load offset=12
              f32.add
              f32.store offset=12
              local.get $l26
              local.get $l64
              local.get $l50
              local.get $l45
              f32.mul
              f32.add
              local.tee $l55
              local.get $l26
              f32.load offset=16
              f32.add
              f32.store offset=16
              local.get $l26
              local.get $l63
              local.get $l47
              local.get $l50
              f32.mul
              f32.add
              local.tee $l57
              local.get $l26
              f32.load offset=20
              f32.add
              f32.store offset=20
              local.get $l26
              local.get $l62
              local.get $l45
              local.get $l49
              local.get $l48
              local.get $l54
              f32.mul
              local.get $l45
              local.get $l53
              f32.mul
              local.get $l52
              local.get $l47
              f32.mul
              f32.add
              f32.add
              local.tee $l53
              f32.neg
              f32.mul
              local.tee $l50
              f32.mul
              f32.add
              local.tee $l45
              local.get $l26
              f32.load offset=24
              f32.add
              f32.store offset=24
              local.get $l26
              local.get $l61
              local.get $l47
              local.get $l50
              f32.mul
              f32.add
              local.tee $l47
              local.get $l26
              f32.load offset=28
              f32.add
              f32.store offset=28
              local.get $l26
              local.get $l60
              local.get $l48
              local.get $l50
              f32.mul
              f32.add
              local.tee $l48
              local.get $l26
              f32.load offset=32
              f32.add
              f32.store offset=32
              local.get $l26
              local.get $l56
              local.get $l53
              local.get $l50
              f32.mul
              f32.sub
              local.tee $l50
              local.get $l26
              f32.load offset=36
              f32.add
              f32.store offset=36
              local.get $l26
              local.get $l49
              local.get $l49
              f32.add
              local.tee $l49
              local.get $l26
              f32.load offset=40
              f32.add
              f32.store offset=40
              local.get $l42
              local.get $l35
              i32.const 44
              i32.mul
              i32.add
              local.tee $p1
              local.get $l44
              local.get $p1
              f32.load
              f32.add
              f32.store
              local.get $p1
              local.get $l58
              local.get $p1
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $p1
              local.get $l46
              local.get $p1
              f32.load offset=8
              f32.add
              f32.store offset=8
              local.get $p1
              local.get $l51
              local.get $p1
              f32.load offset=12
              f32.add
              f32.store offset=12
              local.get $p1
              local.get $l55
              local.get $p1
              f32.load offset=16
              f32.add
              f32.store offset=16
              local.get $p1
              local.get $l57
              local.get $p1
              f32.load offset=20
              f32.add
              f32.store offset=20
              local.get $p1
              local.get $l45
              local.get $p1
              f32.load offset=24
              f32.add
              f32.store offset=24
              local.get $p1
              local.get $l47
              local.get $p1
              f32.load offset=28
              f32.add
              f32.store offset=28
              local.get $p1
              local.get $l48
              local.get $p1
              f32.load offset=32
              f32.add
              f32.store offset=32
              local.get $p1
              local.get $l50
              local.get $p1
              f32.load offset=36
              f32.add
              f32.store offset=36
              local.get $p1
              local.get $l49
              local.get $p1
              f32.load offset=40
              f32.add
              f32.store offset=40
            end
            local.get $l33
            i32.const 1
            i32.add
            local.tee $l33
            local.get $l38
            i32.ne
            br_if $L93
          end
        end
        local.get $l66
        local.get $p4
        i32.ne
        br_if $L91
      end
    end
    block $B101
      local.get $p2
      i32.eqz
      br_if $B101
      local.get $l32
      i32.eqz
      br_if $B101
      i32.const 0
      local.set $l25
      loop $L102
        f32.const 0x0p+0 (;=0;)
        local.set $l52
        local.get $l36
        local.get $p0
        local.get $l25
        i32.const 2
        i32.shl
        i32.add
        local.tee $p1
        i32.const 4
        i32.add
        i32.load
        local.tee $l23
        i32.const 12
        i32.mul
        i32.add
        local.tee $l18
        f32.load offset=8
        local.get $l36
        local.get $p1
        i32.load
        local.tee $l33
        i32.const 12
        i32.mul
        i32.add
        local.tee $l19
        f32.load offset=8
        local.tee $l51
        f32.sub
        local.tee $l47
        local.get $l47
        f32.mul
        local.get $l18
        f32.load
        local.get $l19
        f32.load
        local.tee $l55
        f32.sub
        local.tee $l48
        local.get $l48
        f32.mul
        local.get $l18
        f32.load offset=4
        local.get $l19
        f32.load offset=4
        local.tee $l57
        f32.sub
        local.tee $l49
        local.get $l49
        f32.mul
        f32.add
        f32.add
        local.tee $l44
        local.get $l36
        local.get $p1
        i32.const 8
        i32.add
        i32.load
        local.tee $l26
        i32.const 12
        i32.mul
        i32.add
        local.tee $p1
        f32.load offset=8
        local.get $l51
        f32.sub
        local.tee $l50
        f32.mul
        local.get $l47
        local.get $l50
        f32.mul
        local.get $l48
        local.get $p1
        f32.load
        local.get $l55
        f32.sub
        local.tee $l53
        f32.mul
        local.get $l49
        local.get $p1
        f32.load offset=4
        local.get $l57
        f32.sub
        local.tee $l54
        f32.mul
        f32.add
        f32.add
        local.tee $l45
        local.get $l47
        f32.mul
        f32.sub
        f32.const 0x0p+0 (;=0;)
        f32.const 0x1p+0 (;=1;)
        local.get $l44
        local.get $l50
        local.get $l50
        f32.mul
        local.get $l53
        local.get $l53
        f32.mul
        local.get $l54
        local.get $l54
        f32.mul
        f32.add
        f32.add
        local.tee $l46
        f32.mul
        local.get $l45
        local.get $l45
        f32.mul
        f32.sub
        local.tee $l58
        f32.div
        local.get $l58
        f32.const 0x0p+0 (;=0;)
        f32.eq
        select
        local.tee $l58
        f32.mul
        local.set $l60
        local.get $l46
        local.get $l47
        f32.mul
        local.get $l45
        local.get $l50
        f32.mul
        f32.sub
        local.get $l58
        f32.mul
        local.set $l61
        local.get $l44
        local.get $l54
        f32.mul
        local.get $l45
        local.get $l49
        f32.mul
        f32.sub
        local.get $l58
        f32.mul
        local.set $l62
        local.get $l46
        local.get $l49
        f32.mul
        local.get $l45
        local.get $l54
        f32.mul
        f32.sub
        local.get $l58
        f32.mul
        local.set $l63
        local.get $l44
        local.get $l53
        f32.mul
        local.get $l45
        local.get $l48
        f32.mul
        f32.sub
        local.get $l58
        f32.mul
        local.set $l64
        local.get $l46
        local.get $l48
        f32.mul
        local.get $l45
        local.get $l53
        f32.mul
        f32.sub
        local.get $l58
        f32.mul
        local.set $l65
        local.get $l48
        local.get $l54
        f32.mul
        local.get $l49
        local.get $l53
        f32.mul
        f32.sub
        local.tee $l45
        local.get $l45
        f32.mul
        local.get $l49
        local.get $l50
        f32.mul
        local.get $l47
        local.get $l54
        f32.mul
        f32.sub
        local.tee $l45
        local.get $l45
        f32.mul
        local.get $l47
        local.get $l53
        f32.mul
        local.get $l48
        local.get $l50
        f32.mul
        f32.sub
        local.tee $l45
        local.get $l45
        f32.mul
        f32.add
        f32.add
        f32.sqrt
        f32.const 0x1p-1 (;=0.5;)
        f32.mul
        local.set $l45
        local.get $l41
        local.get $l33
        local.get $l32
        i32.mul
        local.tee $l35
        i32.const 2
        i32.shl
        i32.add
        local.set $l18
        local.get $l41
        local.get $l26
        local.get $l32
        i32.mul
        local.tee $l38
        i32.const 2
        i32.shl
        i32.add
        local.set $l19
        local.get $l41
        local.get $l23
        local.get $l32
        i32.mul
        local.tee $p9
        i32.const 2
        i32.shl
        i32.add
        local.set $l20
        local.get $l51
        f32.neg
        local.set $l67
        local.get $l57
        f32.neg
        local.set $l68
        local.get $l55
        f32.neg
        local.set $l69
        local.get $l15
        i32.const 256
        i32.add
        local.set $p1
        local.get $l32
        local.set $l22
        f32.const 0x0p+0 (;=0;)
        local.set $l54
        f32.const 0x0p+0 (;=0;)
        local.set $l58
        f32.const 0x0p+0 (;=0;)
        local.set $l44
        f32.const 0x0p+0 (;=0;)
        local.set $l46
        f32.const 0x0p+0 (;=0;)
        local.set $l51
        f32.const 0x0p+0 (;=0;)
        local.set $l55
        f32.const 0x0p+0 (;=0;)
        local.set $l57
        f32.const 0x0p+0 (;=0;)
        local.set $l59
        f32.const 0x0p+0 (;=0;)
        local.set $l56
        loop $L103
          local.get $p1
          i32.const 8
          i32.add
          local.get $l45
          local.get $l61
          local.get $l20
          f32.load
          local.get $l18
          f32.load
          local.tee $l50
          f32.sub
          local.tee $l49
          f32.mul
          local.get $l60
          local.get $l19
          f32.load
          local.get $l50
          f32.sub
          local.tee $l53
          f32.mul
          f32.add
          local.tee $l47
          f32.mul
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.get $l45
          local.get $l63
          local.get $l49
          f32.mul
          local.get $l62
          local.get $l53
          f32.mul
          f32.add
          local.tee $l48
          f32.mul
          f32.store
          local.get $p1
          local.get $l45
          local.get $l65
          local.get $l49
          f32.mul
          local.get $l64
          local.get $l53
          f32.mul
          f32.add
          local.tee $l49
          f32.mul
          f32.store
          local.get $p1
          i32.const 12
          i32.add
          local.get $l45
          local.get $l67
          local.get $l47
          f32.mul
          local.get $l68
          local.get $l48
          f32.mul
          local.get $l50
          local.get $l69
          local.get $l49
          f32.mul
          f32.add
          f32.add
          f32.add
          local.tee $l50
          f32.mul
          f32.store
          local.get $l45
          local.get $l47
          local.get $l48
          f32.mul
          f32.mul
          local.get $l51
          f32.add
          local.set $l51
          local.get $l45
          local.get $l47
          local.get $l49
          f32.mul
          f32.mul
          local.get $l46
          f32.add
          local.set $l46
          local.get $l45
          local.get $l48
          local.get $l49
          f32.mul
          f32.mul
          local.get $l44
          f32.add
          local.set $l44
          local.get $l45
          local.get $l50
          local.get $l50
          f32.mul
          f32.mul
          local.get $l56
          f32.add
          local.set $l56
          local.get $l45
          local.get $l47
          local.get $l50
          f32.mul
          f32.mul
          local.get $l59
          f32.add
          local.set $l59
          local.get $l45
          local.get $l48
          local.get $l50
          f32.mul
          f32.mul
          local.get $l57
          f32.add
          local.set $l57
          local.get $l45
          local.get $l49
          local.get $l50
          f32.mul
          f32.mul
          local.get $l55
          f32.add
          local.set $l55
          local.get $l45
          local.get $l47
          local.get $l47
          f32.mul
          f32.mul
          local.get $l58
          f32.add
          local.set $l58
          local.get $l45
          local.get $l48
          local.get $l48
          f32.mul
          f32.mul
          local.get $l54
          f32.add
          local.set $l54
          local.get $l45
          local.get $l49
          local.get $l49
          f32.mul
          f32.mul
          local.get $l52
          f32.add
          local.set $l52
          local.get $l18
          i32.const 4
          i32.add
          local.set $l18
          local.get $l20
          i32.const 4
          i32.add
          local.set $l20
          local.get $l19
          i32.const 4
          i32.add
          local.set $l19
          local.get $p1
          i32.const 16
          i32.add
          local.set $p1
          local.get $l22
          i32.const -1
          i32.add
          local.tee $l22
          br_if $L103
        end
        local.get $l34
        local.get $l33
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l52
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l54
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l58
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l44
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l46
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l51
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l55
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l57
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l59
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l56
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l45
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        local.get $l34
        local.get $l23
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l52
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l54
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l58
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l44
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l46
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l51
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l55
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l57
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l59
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l56
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l45
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        local.get $l34
        local.get $l26
        i32.const 44
        i32.mul
        i32.add
        local.tee $p1
        local.get $l52
        local.get $p1
        f32.load
        f32.add
        f32.store
        local.get $p1
        local.get $l54
        local.get $p1
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $p1
        local.get $l58
        local.get $p1
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $p1
        local.get $l44
        local.get $p1
        f32.load offset=12
        f32.add
        f32.store offset=12
        local.get $p1
        local.get $l46
        local.get $p1
        f32.load offset=16
        f32.add
        f32.store offset=16
        local.get $p1
        local.get $l51
        local.get $p1
        f32.load offset=20
        f32.add
        f32.store offset=20
        local.get $p1
        local.get $l55
        local.get $p1
        f32.load offset=24
        f32.add
        f32.store offset=24
        local.get $p1
        local.get $l57
        local.get $p1
        f32.load offset=28
        f32.add
        f32.store offset=28
        local.get $p1
        local.get $l59
        local.get $p1
        f32.load offset=32
        f32.add
        f32.store offset=32
        local.get $p1
        local.get $l56
        local.get $p1
        f32.load offset=36
        f32.add
        f32.store offset=36
        local.get $p1
        local.get $l45
        local.get $p1
        f32.load offset=40
        f32.add
        f32.store offset=40
        local.get $l43
        local.get $l35
        i32.const 4
        i32.shl
        i32.add
        local.set $l23
        i32.const 0
        local.set $l18
        local.get $l32
        local.set $l20
        loop $L104
          local.get $l23
          local.get $l18
          i32.add
          local.tee $p1
          local.get $l15
          i32.const 256
          i32.add
          local.get $l18
          i32.add
          local.tee $l19
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 4
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 8
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 8
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 12
          i32.add
          local.tee $p1
          local.get $l19
          i32.const 12
          i32.add
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $l18
          i32.const 16
          i32.add
          local.set $l18
          local.get $l20
          i32.const -1
          i32.add
          local.tee $l20
          br_if $L104
        end
        local.get $l43
        local.get $p9
        i32.const 4
        i32.shl
        i32.add
        local.set $l23
        i32.const 0
        local.set $l18
        local.get $l32
        local.set $l20
        loop $L105
          local.get $l23
          local.get $l18
          i32.add
          local.tee $p1
          local.get $l15
          i32.const 256
          i32.add
          local.get $l18
          i32.add
          local.tee $l19
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 4
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 8
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 8
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 12
          i32.add
          local.tee $p1
          local.get $l19
          i32.const 12
          i32.add
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $l18
          i32.const 16
          i32.add
          local.set $l18
          local.get $l20
          i32.const -1
          i32.add
          local.tee $l20
          br_if $L105
        end
        local.get $l43
        local.get $l38
        i32.const 4
        i32.shl
        i32.add
        local.set $l23
        i32.const 0
        local.set $l18
        local.get $l32
        local.set $l20
        loop $L106
          local.get $l23
          local.get $l18
          i32.add
          local.tee $p1
          local.get $l15
          i32.const 256
          i32.add
          local.get $l18
          i32.add
          local.tee $l19
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 4
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 4
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 8
          i32.add
          local.tee $l22
          local.get $l19
          i32.const 8
          i32.add
          f32.load
          local.get $l22
          f32.load
          f32.add
          f32.store
          local.get $p1
          i32.const 12
          i32.add
          local.tee $p1
          local.get $l19
          i32.const 12
          i32.add
          f32.load
          local.get $p1
          f32.load
          f32.add
          f32.store
          local.get $l18
          i32.const 16
          i32.add
          local.set $l18
          local.get $l20
          i32.const -1
          i32.add
          local.tee $l20
          br_if $L106
        end
        local.get $l25
        i32.const 3
        i32.add
        local.tee $l25
        local.get $p2
        i32.lt_u
        br_if $L102
      end
    end
    i32.const 0
    local.set $l19
    block $B107
      block $B108
        local.get $p13
        i32.const 8
        i32.and
        local.tee $l70
        br_if $B108
        f32.const 0x0p+0 (;=0;)
        local.set $l58
        i32.const 0
        local.set $l71
        i32.const 0
        local.set $l72
        i32.const 0
        local.set $l73
        br $B107
      end
      i32.const 0
      local.set $l71
      local.get $l28
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l73
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l73
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      i32.const -1
      local.get $l73
      local.get $p4
      local.get $p0
      local.get $p2
      local.get $l17
      call $f5
      local.tee $l20
      i32.const 4
      i32.shl
      local.get $l20
      i32.const 805306368
      i32.and
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.set $l72
      local.get $l15
      i32.const 156
      i32.add
      local.get $l15
      i32.load offset=252
      local.tee $p1
      i32.const 2
      i32.shl
      i32.add
      local.get $l72
      i32.store
      local.get $l15
      local.get $p1
      i32.const 1
      i32.add
      i32.store offset=252
      local.get $l72
      local.get $l20
      local.get $l73
      local.get $l36
      local.get $p4
      call $f6
      f32.const 0x1.fffffep+127 (;=3.40282e+38;)
      local.set $l58
      local.get $l20
      i32.eqz
      br_if $B107
      local.get $l72
      local.set $p1
      local.get $l20
      local.set $l18
      loop $L109
        local.get $p1
        f32.load
        local.tee $l45
        local.get $l58
        local.get $l58
        local.get $l45
        f32.gt
        select
        local.set $l58
        local.get $p1
        i32.const 4
        i32.add
        local.set $p1
        local.get $l18
        i32.const -1
        i32.add
        local.tee $l18
        br_if $L109
      end
      local.get $l20
      local.set $l71
    end
    block $B110
      local.get $p4
      i32.eqz
      br_if $B110
      local.get $l24
      i32.const 4
      i32.add
      local.set $p1
      local.get $l24
      i32.load
      local.set $l20
      local.get $l29
      local.set $l18
      local.get $p4
      local.set $l22
      i32.const 0
      local.set $l19
      loop $L111
        i32.const 0
        local.get $p1
        i32.load
        local.tee $l23
        local.get $l20
        i32.sub
        local.get $l18
        i32.load8_u
        i32.const 253
        i32.and
        select
        local.get $l19
        i32.add
        local.set $l19
        local.get $l18
        i32.const 1
        i32.add
        local.set $l18
        local.get $p1
        i32.const 4
        i32.add
        local.set $p1
        local.get $l23
        local.set $l20
        local.get $l22
        i32.const -1
        i32.add
        local.tee $l22
        br_if $L111
      end
      local.get $l19
      i32.const 1
      i32.shr_u
      local.set $l19
    end
    i32.const -1
    local.get $p2
    local.get $l19
    i32.sub
    i32.const 3
    i32.add
    local.tee $l74
    i32.const 12
    i32.mul
    local.get $l74
    i32.const 357913941
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l75
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l75
    i32.store
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    i32.const -1
    local.get $l74
    i32.const 2
    i32.shl
    local.get $l74
    i32.const 1073741823
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l76
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l76
    i32.store
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $l28
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l77
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l77
    i32.store
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $p4
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l78
    local.get $l15
    i32.const 156
    i32.add
    local.get $l15
    i32.load offset=252
    local.tee $p1
    i32.const 2
    i32.shl
    i32.add
    local.get $l78
    i32.store
    local.get $l15
    local.get $p1
    i32.const 1
    i32.add
    i32.store offset=252
    local.get $p12
    local.get $p12
    f32.mul
    local.get $l40
    f32.const 0x1p+0 (;=1;)
    local.get $p13
    i32.const 4
    i32.and
    select
    local.tee $l79
    local.get $l79
    f32.mul
    f32.div
    local.set $l59
    f32.const 0x0p+0 (;=0;)
    local.set $l56
    block $B112
      local.get $p2
      local.get $p11
      i32.le_u
      br_if $B112
      block $B113
        local.get $l74
        i32.const 3
        i32.lt_u
        br_if $B113
        local.get $p13
        i32.const 256
        i32.and
        local.set $l80
        local.get $l32
        i32.const 4
        i32.shl
        local.set $l81
        local.get $l75
        i32.const 8
        i32.add
        local.set $l82
        f32.const 0x0p+0 (;=0;)
        local.set $l57
        f32.const 0x0p+0 (;=0;)
        local.set $l56
        loop $L114
          local.get $l15
          i32.const 148
          i32.add
          local.get $p0
          local.get $p2
          local.get $p4
          local.get $l17
          call $f2
          local.get $p0
          local.set $l26
          i32.const 0
          local.set $l28
          i32.const 0
          local.set $l83
          loop $L115
            local.get $p0
            local.get $l83
            i32.const 2
            i32.shl
            i32.add
            local.set $l38
            i32.const 0
            local.set $p1
            loop $L116
              block $B117
                local.get $l17
                local.get $l26
                local.get $p1
                i32.add
                i32.load
                local.tee $l19
                i32.const 2
                i32.shl
                local.tee $l33
                i32.add
                i32.load
                local.tee $l20
                local.get $l17
                local.get $l38
                local.get $p1
                i32.const 36912
                i32.add
                i32.load
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.tee $l18
                i32.const 2
                i32.shl
                local.tee $l35
                i32.add
                i32.load
                local.tee $l22
                i32.eq
                br_if $B117
                local.get $l29
                local.get $l18
                i32.add
                i32.load8_u
                local.tee $l25
                i32.const 6
                i32.mul
                local.get $l29
                local.get $l19
                i32.add
                i32.load8_u
                local.tee $l23
                i32.add
                i32.load8_u offset=36928
                local.tee $l37
                local.get $l23
                i32.const 6
                i32.mul
                local.get $l25
                i32.add
                local.tee $p9
                i32.load8_u offset=36928
                local.tee $l66
                i32.or
                i32.const 255
                i32.and
                i32.eqz
                br_if $B117
                block $B118
                  local.get $l22
                  local.get $l20
                  i32.le_u
                  br_if $B118
                  local.get $p9
                  i32.load8_u offset=36976
                  i32.const 255
                  i32.and
                  br_if $B117
                end
                block $B119
                  local.get $l23
                  i32.const -1
                  i32.add
                  i32.const 255
                  i32.and
                  i32.const 1
                  i32.gt_u
                  br_if $B119
                  local.get $l25
                  i32.eqz
                  br_if $B119
                  local.get $l30
                  local.get $l33
                  i32.add
                  i32.load
                  local.get $l18
                  i32.ne
                  br_if $B117
                end
                block $B120
                  local.get $l23
                  i32.eqz
                  br_if $B120
                  local.get $l25
                  i32.const -1
                  i32.add
                  i32.const 255
                  i32.and
                  i32.const 1
                  i32.gt_u
                  br_if $B120
                  local.get $l31
                  local.get $l35
                  i32.add
                  i32.load
                  local.get $l19
                  i32.ne
                  br_if $B117
                end
                local.get $l75
                local.get $l28
                i32.const 12
                i32.mul
                i32.add
                local.tee $l20
                local.get $l18
                local.get $l19
                local.get $l66
                i32.const 255
                i32.and
                local.tee $l22
                select
                i32.store offset=4
                local.get $l20
                local.get $l19
                local.get $l18
                local.get $l22
                select
                i32.store
                local.get $l20
                local.get $l22
                local.get $l37
                i32.and
                i32.const 0
                i32.ne
                i32.store offset=8
                local.get $l28
                i32.const 1
                i32.add
                local.set $l28
              end
              local.get $p1
              i32.const 4
              i32.add
              local.tee $p1
              i32.const 12
              i32.ne
              br_if $L116
            end
            block $B121
              local.get $l83
              i32.const 3
              i32.add
              local.tee $l83
              local.get $p2
              i32.ge_u
              br_if $B121
              local.get $l26
              i32.const 12
              i32.add
              local.set $l26
              local.get $l28
              i32.const 3
              i32.add
              local.get $l74
              i32.le_u
              br_if $L115
            end
          end
          local.get $l28
          i32.eqz
          br_if $B112
          i32.const 0
          local.set $p9
          loop $L122
            local.get $l42
            local.get $l17
            local.get $l75
            local.get $p9
            i32.const 12
            i32.mul
            i32.add
            local.tee $l33
            i32.load
            local.tee $l23
            i32.const 2
            i32.shl
            local.tee $l20
            i32.add
            i32.load
            local.tee $l35
            i32.const 44
            i32.mul
            i32.add
            local.tee $p1
            f32.load offset=8
            local.get $l36
            local.get $l33
            i32.load offset=4
            local.tee $l25
            i32.const 12
            i32.mul
            i32.add
            local.tee $l18
            f32.load offset=8
            local.tee $l48
            f32.mul
            local.get $p1
            f32.load offset=16
            local.get $l18
            f32.load
            local.tee $l49
            f32.mul
            local.get $p1
            f32.load offset=32
            f32.add
            local.tee $l45
            local.get $l45
            f32.add
            f32.add
            local.get $l48
            f32.mul
            local.get $p1
            f32.load offset=4
            local.get $l18
            f32.load offset=4
            local.tee $l50
            f32.mul
            local.get $p1
            f32.load offset=20
            local.get $l48
            f32.mul
            local.get $p1
            f32.load offset=28
            f32.add
            local.tee $l45
            local.get $l45
            f32.add
            f32.add
            local.get $l50
            f32.mul
            local.get $p1
            f32.load
            local.get $l49
            f32.mul
            local.get $p1
            f32.load offset=12
            local.get $l50
            f32.mul
            local.get $p1
            f32.load offset=24
            f32.add
            local.tee $l45
            local.get $l45
            f32.add
            f32.add
            local.get $l49
            f32.mul
            local.get $p1
            f32.load offset=36
            f32.add
            f32.add
            f32.add
            f32.abs
            local.set $l45
            f32.const 0x0p+0 (;=0;)
            f32.const 0x1p+0 (;=1;)
            local.get $p1
            f32.load offset=40
            local.tee $l47
            f32.div
            local.get $l47
            f32.const 0x0p+0 (;=0;)
            f32.eq
            select
            local.set $l47
            block $B123
              block $B124
                local.get $l33
                i32.load offset=8
                local.tee $l38
                br_if $B124
                f32.const 0x1.fffffep+127 (;=3.40282e+38;)
                local.set $l54
                br $B123
              end
              f32.const 0x0p+0 (;=0;)
              f32.const 0x1p+0 (;=1;)
              local.get $l42
              local.get $l17
              local.get $l25
              i32.const 2
              i32.shl
              i32.add
              i32.load
              i32.const 44
              i32.mul
              i32.add
              local.tee $p1
              f32.load offset=40
              local.tee $l53
              f32.div
              local.get $l53
              f32.const 0x0p+0 (;=0;)
              f32.eq
              select
              local.get $p1
              f32.load offset=8
              local.get $l36
              local.get $l23
              i32.const 12
              i32.mul
              i32.add
              local.tee $l18
              f32.load offset=8
              local.tee $l53
              f32.mul
              local.get $p1
              f32.load offset=16
              local.get $l18
              f32.load
              local.tee $l52
              f32.mul
              local.get $p1
              f32.load offset=32
              f32.add
              local.tee $l54
              local.get $l54
              f32.add
              f32.add
              local.get $l53
              f32.mul
              local.get $p1
              f32.load offset=4
              local.get $l18
              f32.load offset=4
              local.tee $l54
              f32.mul
              local.get $p1
              f32.load offset=20
              local.get $l53
              f32.mul
              local.get $p1
              f32.load offset=28
              f32.add
              local.tee $l53
              local.get $l53
              f32.add
              f32.add
              local.get $l54
              f32.mul
              local.get $p1
              f32.load
              local.get $l52
              f32.mul
              local.get $p1
              f32.load offset=12
              local.get $l54
              f32.mul
              local.get $p1
              f32.load offset=24
              f32.add
              local.tee $l53
              local.get $l53
              f32.add
              f32.add
              local.get $l52
              f32.mul
              local.get $p1
              f32.load offset=36
              f32.add
              f32.add
              f32.add
              f32.abs
              f32.mul
              local.set $l54
            end
            local.get $l47
            local.get $l45
            f32.mul
            local.set $l52
            block $B125
              local.get $l32
              i32.eqz
              br_if $B125
              local.get $l34
              local.get $l23
              i32.const 44
              i32.mul
              i32.add
              local.tee $l19
              f32.load offset=8
              local.get $l48
              f32.mul
              local.get $l19
              f32.load offset=16
              local.get $l49
              f32.mul
              local.get $l19
              f32.load offset=32
              f32.add
              local.tee $l45
              local.get $l45
              f32.add
              f32.add
              local.get $l48
              f32.mul
              local.get $l19
              f32.load offset=4
              local.get $l50
              f32.mul
              local.get $l19
              f32.load offset=20
              local.get $l48
              f32.mul
              local.get $l19
              f32.load offset=28
              f32.add
              local.tee $l45
              local.get $l45
              f32.add
              f32.add
              local.get $l50
              f32.mul
              local.get $l19
              f32.load
              local.get $l49
              f32.mul
              local.get $l19
              f32.load offset=12
              local.get $l50
              f32.mul
              local.get $l19
              f32.load offset=24
              f32.add
              local.tee $l45
              local.get $l45
              f32.add
              f32.add
              local.get $l49
              f32.mul
              local.get $l19
              f32.load offset=36
              f32.add
              f32.add
              f32.add
              local.set $l45
              local.get $l41
              local.get $l25
              local.get $l32
              i32.mul
              local.tee $l22
              i32.const 2
              i32.shl
              i32.add
              local.set $l18
              local.get $l43
              local.get $l23
              local.get $l32
              i32.mul
              local.tee $l26
              i32.const 4
              i32.shl
              i32.add
              local.set $p1
              local.get $l19
              f32.load offset=40
              local.set $l53
              local.get $l32
              local.set $l19
              loop $L126
                local.get $l18
                f32.load
                local.tee $l47
                local.get $l47
                local.get $l53
                f32.mul
                local.get $p1
                i32.const 12
                i32.add
                f32.load
                local.get $l48
                local.get $p1
                i32.const 8
                i32.add
                f32.load
                f32.mul
                local.get $l49
                local.get $p1
                f32.load
                f32.mul
                local.get $l50
                local.get $p1
                i32.const 4
                i32.add
                f32.load
                f32.mul
                f32.add
                f32.add
                f32.add
                local.tee $l47
                local.get $l47
                f32.add
                f32.sub
                f32.mul
                local.get $l45
                f32.add
                local.set $l45
                local.get $l18
                i32.const 4
                i32.add
                local.set $l18
                local.get $p1
                i32.const 16
                i32.add
                local.set $p1
                local.get $l19
                i32.const -1
                i32.add
                local.tee $l19
                br_if $L126
              end
              local.get $l45
              f32.abs
              local.tee $l45
              local.get $l53
              local.get $l45
              local.get $l53
              f32.lt
              select
              local.get $l45
              local.get $l80
              select
              local.set $l44
              block $B127
                block $B128
                  local.get $l38
                  br_if $B128
                  f32.const 0x0p+0 (;=0;)
                  local.set $l45
                  br $B127
                end
                local.get $l34
                local.get $l25
                i32.const 44
                i32.mul
                i32.add
                local.tee $l19
                f32.load offset=8
                local.get $l36
                local.get $l23
                i32.const 12
                i32.mul
                i32.add
                local.tee $p1
                f32.load offset=8
                local.tee $l48
                f32.mul
                local.get $l19
                f32.load offset=16
                local.get $p1
                f32.load
                local.tee $l49
                f32.mul
                local.get $l19
                f32.load offset=32
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l48
                f32.mul
                local.get $l19
                f32.load offset=4
                local.get $p1
                f32.load offset=4
                local.tee $l50
                f32.mul
                local.get $l19
                f32.load offset=20
                local.get $l48
                f32.mul
                local.get $l19
                f32.load offset=28
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l50
                f32.mul
                local.get $l19
                f32.load
                local.get $l49
                f32.mul
                local.get $l19
                f32.load offset=12
                local.get $l50
                f32.mul
                local.get $l19
                f32.load offset=24
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l49
                f32.mul
                local.get $l19
                f32.load offset=36
                f32.add
                f32.add
                f32.add
                local.set $l45
                local.get $l41
                local.get $l26
                i32.const 2
                i32.shl
                i32.add
                local.set $l18
                local.get $l43
                local.get $l22
                i32.const 4
                i32.shl
                i32.add
                local.set $p1
                local.get $l19
                f32.load offset=40
                local.set $l53
                local.get $l32
                local.set $l19
                loop $L129
                  local.get $l18
                  f32.load
                  local.tee $l47
                  local.get $l47
                  local.get $l53
                  f32.mul
                  local.get $p1
                  i32.const 12
                  i32.add
                  f32.load
                  local.get $l48
                  local.get $p1
                  i32.const 8
                  i32.add
                  f32.load
                  f32.mul
                  local.get $l49
                  local.get $p1
                  f32.load
                  f32.mul
                  local.get $l50
                  local.get $p1
                  i32.const 4
                  i32.add
                  f32.load
                  f32.mul
                  f32.add
                  f32.add
                  f32.add
                  local.tee $l47
                  local.get $l47
                  f32.add
                  f32.sub
                  f32.mul
                  local.get $l45
                  f32.add
                  local.set $l45
                  local.get $l18
                  i32.const 4
                  i32.add
                  local.set $l18
                  local.get $p1
                  i32.const 16
                  i32.add
                  local.set $p1
                  local.get $l19
                  i32.const -1
                  i32.add
                  local.tee $l19
                  br_if $L129
                end
                local.get $l45
                f32.abs
                local.tee $l45
                local.get $l53
                local.get $l45
                local.get $l53
                f32.lt
                select
                local.get $l45
                local.get $l80
                select
                local.set $l45
              end
              local.get $l52
              local.get $l44
              f32.add
              local.set $l52
              local.get $l54
              local.get $l45
              f32.add
              local.set $l54
              block $B130
                block $B131
                  block $B132
                    local.get $l29
                    local.get $l23
                    i32.add
                    i32.load8_u
                    i32.const -2
                    i32.add
                    br_table $B132 $B131 $B131 $B130
                  end
                  block $B133
                    local.get $l31
                    local.get $l30
                    local.get $l30
                    local.get $l20
                    i32.add
                    i32.load
                    local.get $l25
                    i32.eq
                    select
                    local.get $l21
                    local.get $l20
                    i32.add
                    i32.load
                    local.tee $l22
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.tee $l20
                    i32.const -1
                    i32.ne
                    br_if $B133
                    local.get $l21
                    local.get $l25
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.set $l20
                  end
                  local.get $l34
                  local.get $l22
                  i32.const 44
                  i32.mul
                  i32.add
                  local.tee $l19
                  f32.load offset=8
                  local.get $l36
                  local.get $l20
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $p1
                  f32.load offset=8
                  local.tee $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=16
                  local.get $p1
                  f32.load
                  local.tee $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=32
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=4
                  local.get $p1
                  f32.load offset=4
                  local.tee $l50
                  f32.mul
                  local.get $l19
                  f32.load offset=20
                  local.get $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=28
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l50
                  f32.mul
                  local.get $l19
                  f32.load
                  local.get $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=12
                  local.get $l50
                  f32.mul
                  local.get $l19
                  f32.load offset=24
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=36
                  f32.add
                  f32.add
                  f32.add
                  local.set $l45
                  local.get $l41
                  local.get $l20
                  local.get $l32
                  i32.mul
                  local.tee $l26
                  i32.const 2
                  i32.shl
                  i32.add
                  local.set $l18
                  local.get $l43
                  local.get $l22
                  local.get $l32
                  i32.mul
                  local.tee $l35
                  i32.const 4
                  i32.shl
                  i32.add
                  local.set $p1
                  local.get $l19
                  f32.load offset=40
                  local.set $l53
                  local.get $l32
                  local.set $l19
                  loop $L134
                    local.get $l18
                    f32.load
                    local.tee $l47
                    local.get $l47
                    local.get $l53
                    f32.mul
                    local.get $p1
                    i32.const 12
                    i32.add
                    f32.load
                    local.get $l48
                    local.get $p1
                    i32.const 8
                    i32.add
                    f32.load
                    f32.mul
                    local.get $l49
                    local.get $p1
                    f32.load
                    f32.mul
                    local.get $l50
                    local.get $p1
                    i32.const 4
                    i32.add
                    f32.load
                    f32.mul
                    f32.add
                    f32.add
                    f32.add
                    local.tee $l47
                    local.get $l47
                    f32.add
                    f32.sub
                    f32.mul
                    local.get $l45
                    f32.add
                    local.set $l45
                    local.get $l18
                    i32.const 4
                    i32.add
                    local.set $l18
                    local.get $p1
                    i32.const 16
                    i32.add
                    local.set $p1
                    local.get $l19
                    i32.const -1
                    i32.add
                    local.tee $l19
                    br_if $L134
                  end
                  local.get $l45
                  f32.abs
                  local.tee $l45
                  local.get $l53
                  local.get $l45
                  local.get $l53
                  f32.lt
                  select
                  local.get $l45
                  local.get $l80
                  select
                  local.set $l44
                  block $B135
                    block $B136
                      local.get $l38
                      br_if $B136
                      f32.const 0x0p+0 (;=0;)
                      local.set $l45
                      br $B135
                    end
                    local.get $l34
                    local.get $l20
                    i32.const 44
                    i32.mul
                    i32.add
                    local.tee $l19
                    f32.load offset=8
                    local.get $l36
                    local.get $l22
                    i32.const 12
                    i32.mul
                    i32.add
                    local.tee $p1
                    f32.load offset=8
                    local.tee $l48
                    f32.mul
                    local.get $l19
                    f32.load offset=16
                    local.get $p1
                    f32.load
                    local.tee $l49
                    f32.mul
                    local.get $l19
                    f32.load offset=32
                    f32.add
                    local.tee $l45
                    local.get $l45
                    f32.add
                    f32.add
                    local.get $l48
                    f32.mul
                    local.get $l19
                    f32.load offset=4
                    local.get $p1
                    f32.load offset=4
                    local.tee $l50
                    f32.mul
                    local.get $l19
                    f32.load offset=20
                    local.get $l48
                    f32.mul
                    local.get $l19
                    f32.load offset=28
                    f32.add
                    local.tee $l45
                    local.get $l45
                    f32.add
                    f32.add
                    local.get $l50
                    f32.mul
                    local.get $l19
                    f32.load
                    local.get $l49
                    f32.mul
                    local.get $l19
                    f32.load offset=12
                    local.get $l50
                    f32.mul
                    local.get $l19
                    f32.load offset=24
                    f32.add
                    local.tee $l45
                    local.get $l45
                    f32.add
                    f32.add
                    local.get $l49
                    f32.mul
                    local.get $l19
                    f32.load offset=36
                    f32.add
                    f32.add
                    f32.add
                    local.set $l45
                    local.get $l41
                    local.get $l35
                    i32.const 2
                    i32.shl
                    i32.add
                    local.set $l18
                    local.get $l43
                    local.get $l26
                    i32.const 4
                    i32.shl
                    i32.add
                    local.set $p1
                    local.get $l19
                    f32.load offset=40
                    local.set $l53
                    local.get $l32
                    local.set $l19
                    loop $L137
                      local.get $l18
                      f32.load
                      local.tee $l47
                      local.get $l47
                      local.get $l53
                      f32.mul
                      local.get $p1
                      i32.const 12
                      i32.add
                      f32.load
                      local.get $l48
                      local.get $p1
                      i32.const 8
                      i32.add
                      f32.load
                      f32.mul
                      local.get $l49
                      local.get $p1
                      f32.load
                      f32.mul
                      local.get $l50
                      local.get $p1
                      i32.const 4
                      i32.add
                      f32.load
                      f32.mul
                      f32.add
                      f32.add
                      f32.add
                      local.tee $l47
                      local.get $l47
                      f32.add
                      f32.sub
                      f32.mul
                      local.get $l45
                      f32.add
                      local.set $l45
                      local.get $l18
                      i32.const 4
                      i32.add
                      local.set $l18
                      local.get $p1
                      i32.const 16
                      i32.add
                      local.set $p1
                      local.get $l19
                      i32.const -1
                      i32.add
                      local.tee $l19
                      br_if $L137
                    end
                    local.get $l45
                    f32.abs
                    local.tee $l45
                    local.get $l53
                    local.get $l45
                    local.get $l53
                    f32.lt
                    select
                    local.get $l45
                    local.get $l80
                    select
                    local.set $l45
                  end
                  local.get $l52
                  local.get $l44
                  f32.add
                  local.set $l52
                  local.get $l54
                  local.get $l45
                  f32.add
                  local.set $l54
                  br $B125
                end
                local.get $l21
                local.get $l20
                i32.add
                i32.load
                local.tee $l20
                local.get $l23
                i32.eq
                br_if $B130
                local.get $l17
                local.get $l25
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.set $l26
                loop $L138
                  block $B139
                    block $B140
                      local.get $l30
                      local.get $l20
                      i32.const 2
                      i32.shl
                      local.tee $l22
                      i32.add
                      i32.load
                      local.tee $p1
                      i32.const -1
                      i32.eq
                      br_if $B140
                      local.get $l17
                      local.get $p1
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      local.get $l26
                      i32.eq
                      br_if $B139
                    end
                    block $B141
                      local.get $l31
                      local.get $l22
                      i32.add
                      i32.load
                      local.tee $p1
                      i32.const -1
                      i32.eq
                      br_if $B141
                      local.get $l17
                      local.get $p1
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      local.get $l26
                      i32.eq
                      br_if $B139
                    end
                    local.get $l25
                    local.set $p1
                  end
                  local.get $l34
                  local.get $l20
                  i32.const 44
                  i32.mul
                  i32.add
                  local.tee $l19
                  f32.load offset=8
                  local.get $l36
                  local.get $p1
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $l18
                  f32.load offset=8
                  local.tee $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=16
                  local.get $l18
                  f32.load
                  local.tee $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=32
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=4
                  local.get $l18
                  f32.load offset=4
                  local.tee $l50
                  f32.mul
                  local.get $l19
                  f32.load offset=20
                  local.get $l48
                  f32.mul
                  local.get $l19
                  f32.load offset=28
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l50
                  f32.mul
                  local.get $l19
                  f32.load
                  local.get $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=12
                  local.get $l50
                  f32.mul
                  local.get $l19
                  f32.load offset=24
                  f32.add
                  local.tee $l45
                  local.get $l45
                  f32.add
                  f32.add
                  local.get $l49
                  f32.mul
                  local.get $l19
                  f32.load offset=36
                  f32.add
                  f32.add
                  f32.add
                  local.set $l45
                  local.get $l41
                  local.get $p1
                  local.get $l32
                  i32.mul
                  i32.const 2
                  i32.shl
                  i32.add
                  local.set $l18
                  local.get $l43
                  local.get $l20
                  local.get $l32
                  i32.mul
                  i32.const 4
                  i32.shl
                  i32.add
                  local.set $p1
                  local.get $l19
                  f32.load offset=40
                  local.set $l53
                  local.get $l32
                  local.set $l19
                  loop $L142
                    local.get $l18
                    f32.load
                    local.tee $l47
                    local.get $l47
                    local.get $l53
                    f32.mul
                    local.get $p1
                    i32.const 12
                    i32.add
                    f32.load
                    local.get $l48
                    local.get $p1
                    i32.const 8
                    i32.add
                    f32.load
                    f32.mul
                    local.get $l49
                    local.get $p1
                    f32.load
                    f32.mul
                    local.get $l50
                    local.get $p1
                    i32.const 4
                    i32.add
                    f32.load
                    f32.mul
                    f32.add
                    f32.add
                    f32.add
                    local.tee $l47
                    local.get $l47
                    f32.add
                    f32.sub
                    f32.mul
                    local.get $l45
                    f32.add
                    local.set $l45
                    local.get $l18
                    i32.const 4
                    i32.add
                    local.set $l18
                    local.get $p1
                    i32.const 16
                    i32.add
                    local.set $p1
                    local.get $l19
                    i32.const -1
                    i32.add
                    local.tee $l19
                    br_if $L142
                  end
                  local.get $l52
                  local.get $l45
                  f32.abs
                  local.tee $l45
                  local.get $l53
                  local.get $l45
                  local.get $l53
                  f32.lt
                  select
                  local.get $l45
                  local.get $l80
                  select
                  f32.add
                  local.set $l52
                  local.get $l21
                  local.get $l22
                  i32.add
                  i32.load
                  local.tee $l20
                  local.get $l23
                  i32.ne
                  br_if $L138
                end
              end
              block $B143
                local.get $l29
                local.get $l25
                i32.add
                i32.load8_u
                local.tee $p1
                i32.const 3
                i32.eq
                br_if $B143
                local.get $p1
                i32.const 4
                i32.ne
                br_if $B125
              end
              local.get $l38
              i32.eqz
              br_if $B125
              local.get $l21
              local.get $l25
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l20
              local.get $l25
              i32.eq
              br_if $B125
              loop $L144
                block $B145
                  block $B146
                    local.get $l30
                    local.get $l20
                    i32.const 2
                    i32.shl
                    local.tee $l22
                    i32.add
                    i32.load
                    local.tee $p1
                    i32.const -1
                    i32.eq
                    br_if $B146
                    local.get $l17
                    local.get $p1
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.get $l35
                    i32.eq
                    br_if $B145
                  end
                  block $B147
                    local.get $l31
                    local.get $l22
                    i32.add
                    i32.load
                    local.tee $p1
                    i32.const -1
                    i32.eq
                    br_if $B147
                    local.get $l17
                    local.get $p1
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.get $l35
                    i32.eq
                    br_if $B145
                  end
                  local.get $l23
                  local.set $p1
                end
                local.get $l34
                local.get $l20
                i32.const 44
                i32.mul
                i32.add
                local.tee $l19
                f32.load offset=8
                local.get $l36
                local.get $p1
                i32.const 12
                i32.mul
                i32.add
                local.tee $l18
                f32.load offset=8
                local.tee $l48
                f32.mul
                local.get $l19
                f32.load offset=16
                local.get $l18
                f32.load
                local.tee $l49
                f32.mul
                local.get $l19
                f32.load offset=32
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l48
                f32.mul
                local.get $l19
                f32.load offset=4
                local.get $l18
                f32.load offset=4
                local.tee $l50
                f32.mul
                local.get $l19
                f32.load offset=20
                local.get $l48
                f32.mul
                local.get $l19
                f32.load offset=28
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l50
                f32.mul
                local.get $l19
                f32.load
                local.get $l49
                f32.mul
                local.get $l19
                f32.load offset=12
                local.get $l50
                f32.mul
                local.get $l19
                f32.load offset=24
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l49
                f32.mul
                local.get $l19
                f32.load offset=36
                f32.add
                f32.add
                f32.add
                local.set $l45
                local.get $l41
                local.get $p1
                local.get $l32
                i32.mul
                i32.const 2
                i32.shl
                i32.add
                local.set $l18
                local.get $l43
                local.get $l20
                local.get $l32
                i32.mul
                i32.const 4
                i32.shl
                i32.add
                local.set $p1
                local.get $l19
                f32.load offset=40
                local.set $l53
                local.get $l32
                local.set $l19
                loop $L148
                  local.get $l18
                  f32.load
                  local.tee $l47
                  local.get $l47
                  local.get $l53
                  f32.mul
                  local.get $p1
                  i32.const 12
                  i32.add
                  f32.load
                  local.get $l48
                  local.get $p1
                  i32.const 8
                  i32.add
                  f32.load
                  f32.mul
                  local.get $l49
                  local.get $p1
                  f32.load
                  f32.mul
                  local.get $l50
                  local.get $p1
                  i32.const 4
                  i32.add
                  f32.load
                  f32.mul
                  f32.add
                  f32.add
                  f32.add
                  local.tee $l47
                  local.get $l47
                  f32.add
                  f32.sub
                  f32.mul
                  local.get $l45
                  f32.add
                  local.set $l45
                  local.get $l18
                  i32.const 4
                  i32.add
                  local.set $l18
                  local.get $p1
                  i32.const 16
                  i32.add
                  local.set $p1
                  local.get $l19
                  i32.const -1
                  i32.add
                  local.tee $l19
                  br_if $L148
                end
                local.get $l54
                local.get $l45
                f32.abs
                local.tee $l45
                local.get $l53
                local.get $l45
                local.get $l53
                f32.lt
                select
                local.get $l45
                local.get $l80
                select
                f32.add
                local.set $l54
                local.get $l21
                local.get $l22
                i32.add
                i32.load
                local.tee $l20
                local.get $l25
                i32.ne
                br_if $L144
              end
            end
            local.get $l33
            local.get $l54
            local.get $l52
            local.get $l54
            local.get $l52
            f32.lt
            local.tee $p1
            select
            f32.store offset=8
            local.get $l33
            local.get $l23
            local.get $l25
            local.get $p1
            local.get $l38
            i32.const 0
            i32.ne
            i32.and
            local.tee $p1
            select
            i32.store offset=4
            local.get $l33
            local.get $l25
            local.get $l23
            local.get $p1
            select
            i32.store
            local.get $p9
            i32.const 1
            i32.add
            local.tee $p9
            local.get $l28
            i32.ne
            br_if $L122
          end
          local.get $l15
          i32.const 256
          i32.add
          i32.const 0
          i32.const 10240
          call $f23
          drop
          local.get $l82
          local.set $p1
          local.get $l28
          local.set $l18
          loop $L149
            local.get $l15
            i32.const 256
            i32.add
            local.get $p1
            i32.load
            i32.const 19
            i32.shr_u
            i32.const 4095
            i32.and
            local.tee $l19
            i32.const 2559
            local.get $l19
            i32.const 2559
            i32.lt_u
            select
            i32.const 2
            i32.shl
            i32.add
            local.tee $l19
            local.get $l19
            i32.load
            i32.const 1
            i32.add
            i32.store
            local.get $p1
            i32.const 12
            i32.add
            local.set $p1
            local.get $l18
            i32.const -1
            i32.add
            local.tee $l18
            br_if $L149
          end
          i32.const 0
          local.set $p1
          i32.const 0
          local.set $l18
          loop $L150
            local.get $l15
            i32.const 256
            i32.add
            local.get $p1
            i32.add
            local.tee $l19
            i32.load
            local.set $l20
            local.get $l19
            local.get $l18
            i32.store
            local.get $l20
            local.get $l18
            i32.add
            local.set $l18
            local.get $p1
            i32.const 4
            i32.add
            local.tee $p1
            i32.const 10240
            i32.ne
            br_if $L150
          end
          i32.const 0
          local.set $p1
          local.get $l82
          local.set $l18
          loop $L151
            local.get $l15
            i32.const 256
            i32.add
            local.get $l18
            i32.load
            i32.const 19
            i32.shr_u
            i32.const 4095
            i32.and
            local.tee $l19
            i32.const 2559
            local.get $l19
            i32.const 2559
            i32.lt_u
            select
            i32.const 2
            i32.shl
            i32.add
            local.tee $l19
            local.get $l19
            i32.load
            local.tee $l19
            i32.const 1
            i32.add
            i32.store
            local.get $l76
            local.get $l19
            i32.const 2
            i32.shl
            i32.add
            local.get $p1
            i32.store
            local.get $l18
            i32.const 12
            i32.add
            local.set $l18
            local.get $l28
            local.get $p1
            i32.const 1
            i32.add
            local.tee $p1
            i32.ne
            br_if $L151
          end
          local.get $p2
          local.get $p11
          i32.sub
          local.tee $l19
          i32.const 3
          i32.div_u
          local.set $l84
          block $B152
            local.get $p4
            i32.eqz
            br_if $B152
            i32.const 0
            local.set $p1
            local.get $l77
            local.set $l18
            loop $L153
              local.get $l18
              local.get $p1
              i32.store
              local.get $l18
              i32.const 4
              i32.add
              local.set $l18
              local.get $p4
              local.get $p1
              i32.const 1
              i32.add
              local.tee $p1
              i32.ne
              br_if $L153
            end
          end
          i32.const 0
          local.set $l85
          local.get $l78
          i32.const 0
          local.get $p4
          call $f23
          local.set $l66
          local.get $l19
          i32.const 18
          i32.div_u
          local.set $l86
          local.get $l84
          i32.const 1
          i32.shr_u
          local.set $l83
          i32.const 0
          local.set $l37
          i32.const 0
          local.set $l38
          block $B154
            loop $L155
              local.get $l75
              local.get $l76
              local.get $l38
              i32.const 2
              i32.shl
              i32.add
              i32.load
              i32.const 12
              i32.mul
              i32.add
              local.tee $l35
              f32.load offset=8
              local.tee $l45
              local.get $l59
              f32.gt
              br_if $B154
              local.get $l37
              local.get $l84
              i32.ge_u
              br_if $B154
              f32.const 0x1.fffffep+127 (;=3.40282e+38;)
              local.set $l47
              block $B156
                local.get $l83
                local.get $l28
                i32.ge_u
                br_if $B156
                local.get $l75
                local.get $l76
                local.get $l83
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.const 12
                i32.mul
                i32.add
                f32.load offset=8
                f32.const 0x1.8p+0 (;=1.5;)
                f32.mul
                local.set $l47
              end
              block $B157
                local.get $l45
                local.get $l47
                f32.gt
                i32.eqz
                br_if $B157
                local.get $l45
                local.get $l56
                f32.gt
                i32.eqz
                br_if $B157
                local.get $l37
                local.get $l86
                i32.gt_u
                br_if $B154
              end
              block $B158
                local.get $l66
                local.get $l17
                local.get $l35
                i32.load offset=4
                local.tee $p9
                i32.const 2
                i32.shl
                local.tee $l87
                i32.add
                local.tee $l26
                i32.load
                local.tee $l19
                i32.add
                local.tee $l88
                i32.load8_u
                local.get $l66
                local.get $l17
                local.get $l35
                i32.load
                local.tee $l33
                i32.const 2
                i32.shl
                local.tee $l89
                i32.add
                i32.load
                local.tee $p1
                i32.add
                local.tee $l90
                i32.load8_u
                i32.or
                br_if $B158
                local.get $l29
                local.get $l33
                i32.add
                i32.load8_u
                local.set $l91
                block $B159
                  block $B160
                    local.get $l24
                    local.get $p1
                    i32.const 2
                    i32.shl
                    i32.add
                    local.tee $l18
                    i32.const 4
                    i32.add
                    i32.load
                    local.tee $l20
                    local.get $l18
                    i32.load
                    local.tee $l18
                    i32.eq
                    br_if $B160
                    local.get $l20
                    local.get $l18
                    i32.sub
                    local.set $l20
                    local.get $l36
                    local.get $l19
                    i32.const 12
                    i32.mul
                    i32.add
                    local.set $l23
                    local.get $l36
                    local.get $p1
                    i32.const 12
                    i32.mul
                    i32.add
                    local.set $l25
                    local.get $l27
                    local.get $l18
                    i32.const 3
                    i32.shl
                    i32.add
                    local.set $p1
                    loop $L161
                      block $B162
                        local.get $l77
                        local.get $p1
                        i32.load
                        i32.const 2
                        i32.shl
                        i32.add
                        i32.load
                        local.tee $l18
                        local.get $l19
                        i32.eq
                        br_if $B162
                        local.get $l77
                        local.get $p1
                        i32.const 4
                        i32.add
                        i32.load
                        i32.const 2
                        i32.shl
                        i32.add
                        i32.load
                        local.tee $l22
                        local.get $l19
                        i32.eq
                        br_if $B162
                        local.get $l18
                        local.get $l22
                        i32.eq
                        br_if $B162
                        local.get $l36
                        local.get $l22
                        i32.const 12
                        i32.mul
                        i32.add
                        local.tee $l22
                        f32.load
                        local.get $l36
                        local.get $l18
                        i32.const 12
                        i32.mul
                        i32.add
                        local.tee $l18
                        f32.load
                        local.tee $l48
                        f32.sub
                        local.tee $l45
                        local.get $l25
                        f32.load offset=4
                        local.get $l18
                        f32.load offset=4
                        local.tee $l49
                        f32.sub
                        local.tee $l52
                        f32.mul
                        local.get $l22
                        f32.load offset=4
                        local.get $l49
                        f32.sub
                        local.tee $l47
                        local.get $l25
                        f32.load
                        local.get $l48
                        f32.sub
                        local.tee $l54
                        f32.mul
                        f32.sub
                        local.tee $l50
                        local.get $l45
                        local.get $l23
                        f32.load offset=4
                        local.get $l49
                        f32.sub
                        local.tee $l44
                        f32.mul
                        local.get $l47
                        local.get $l23
                        f32.load
                        local.get $l48
                        f32.sub
                        local.tee $l46
                        f32.mul
                        f32.sub
                        local.tee $l49
                        f32.mul
                        local.get $l47
                        local.get $l25
                        f32.load offset=8
                        local.get $l18
                        f32.load offset=8
                        local.tee $l53
                        f32.sub
                        local.tee $l51
                        f32.mul
                        local.get $l22
                        f32.load offset=8
                        local.get $l53
                        f32.sub
                        local.tee $l48
                        local.get $l52
                        f32.mul
                        f32.sub
                        local.tee $l52
                        local.get $l47
                        local.get $l23
                        f32.load offset=8
                        local.get $l53
                        f32.sub
                        local.tee $l55
                        f32.mul
                        local.get $l48
                        local.get $l44
                        f32.mul
                        f32.sub
                        local.tee $l47
                        f32.mul
                        local.get $l48
                        local.get $l54
                        f32.mul
                        local.get $l45
                        local.get $l51
                        f32.mul
                        f32.sub
                        local.tee $l53
                        local.get $l48
                        local.get $l46
                        f32.mul
                        local.get $l45
                        local.get $l55
                        f32.mul
                        f32.sub
                        local.tee $l45
                        f32.mul
                        f32.add
                        f32.add
                        local.get $l50
                        local.get $l50
                        f32.mul
                        local.get $l52
                        local.get $l52
                        f32.mul
                        local.get $l53
                        local.get $l53
                        f32.mul
                        f32.add
                        f32.add
                        local.get $l49
                        local.get $l49
                        f32.mul
                        local.get $l47
                        local.get $l47
                        f32.mul
                        local.get $l45
                        local.get $l45
                        f32.mul
                        f32.add
                        f32.add
                        f32.mul
                        f32.sqrt
                        f32.const 0x1p-2 (;=0.25;)
                        f32.mul
                        f32.le
                        br_if $B159
                      end
                      local.get $p1
                      i32.const 8
                      i32.add
                      local.set $p1
                      local.get $l20
                      i32.const -1
                      i32.add
                      local.tee $l20
                      br_if $L161
                    end
                  end
                  block $B163
                    block $B164
                      local.get $l91
                      i32.const -3
                      i32.add
                      i32.const 255
                      i32.and
                      i32.const 1
                      i32.gt_u
                      br_if $B164
                      local.get $l77
                      local.get $l89
                      i32.add
                      local.get $p9
                      i32.store
                      local.get $l21
                      local.get $l89
                      i32.add
                      i32.load
                      local.tee $p1
                      local.get $l33
                      i32.eq
                      br_if $B163
                      loop $L165
                        local.get $l26
                        i32.load
                        local.set $l19
                        block $B166
                          block $B167
                            local.get $l30
                            local.get $p1
                            i32.const 2
                            i32.shl
                            local.tee $l18
                            i32.add
                            i32.load
                            local.tee $p1
                            i32.const -1
                            i32.eq
                            br_if $B167
                            local.get $l17
                            local.get $p1
                            i32.const 2
                            i32.shl
                            i32.add
                            i32.load
                            local.get $l19
                            i32.eq
                            br_if $B166
                          end
                          block $B168
                            local.get $l31
                            local.get $l18
                            i32.add
                            i32.load
                            local.tee $p1
                            i32.const -1
                            i32.eq
                            br_if $B168
                            local.get $l17
                            local.get $p1
                            i32.const 2
                            i32.shl
                            i32.add
                            i32.load
                            local.get $l19
                            i32.eq
                            br_if $B166
                          end
                          local.get $p9
                          local.set $p1
                        end
                        local.get $l77
                        local.get $l18
                        i32.add
                        local.get $p1
                        i32.store
                        local.get $l21
                        local.get $l18
                        i32.add
                        i32.load
                        local.tee $p1
                        local.get $l33
                        i32.ne
                        br_if $L165
                        br $B163
                      end
                    end
                    block $B169
                      block $B170
                        local.get $l91
                        i32.const 255
                        i32.and
                        i32.const 2
                        i32.eq
                        br_if $B170
                        local.get $p9
                        local.set $p1
                        br $B169
                      end
                      block $B171
                        local.get $l31
                        local.get $l30
                        local.get $l30
                        local.get $l89
                        i32.add
                        i32.load
                        local.get $p9
                        i32.eq
                        select
                        local.get $l21
                        local.get $l89
                        i32.add
                        i32.load
                        local.tee $l33
                        i32.const 2
                        i32.shl
                        i32.add
                        i32.load
                        local.tee $p1
                        i32.const -1
                        i32.ne
                        br_if $B171
                        local.get $l21
                        local.get $l87
                        i32.add
                        i32.load
                        local.set $p1
                      end
                      local.get $l77
                      local.get $l89
                      i32.add
                      local.get $p9
                      i32.store
                    end
                    local.get $l77
                    local.get $l33
                    i32.const 2
                    i32.shl
                    i32.add
                    local.get $p1
                    i32.store
                  end
                  local.get $l90
                  i32.const 1
                  i32.store8
                  local.get $l88
                  i32.const 1
                  i32.store8
                  local.get $l35
                  f32.load offset=8
                  local.tee $l45
                  local.get $l56
                  local.get $l56
                  local.get $l45
                  f32.lt
                  select
                  local.set $l56
                  local.get $l85
                  i32.const 1
                  i32.add
                  local.set $l85
                  i32.const 1
                  i32.const 2
                  local.get $l91
                  i32.const 255
                  i32.and
                  i32.const 1
                  i32.eq
                  select
                  local.get $l37
                  i32.add
                  local.set $l37
                  br $B158
                end
                local.get $l83
                i32.const 1
                i32.add
                local.set $l83
              end
              local.get $l38
              i32.const 1
              i32.add
              local.tee $l38
              local.get $l28
              i32.ne
              br_if $L155
            end
          end
          local.get $l85
          i32.eqz
          br_if $B112
          block $B172
            local.get $p4
            i32.eqz
            br_if $B172
            i32.const 0
            local.set $l23
            i32.const 0
            local.set $l33
            loop $L173
              block $B174
                local.get $l77
                local.get $l33
                i32.const 2
                i32.shl
                local.tee $p1
                i32.add
                i32.load
                local.tee $l19
                local.get $l33
                i32.eq
                br_if $B174
                local.get $l17
                local.get $l19
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.set $l26
                block $B175
                  local.get $l33
                  local.get $l17
                  local.get $p1
                  i32.add
                  i32.load
                  i32.ne
                  local.tee $l35
                  br_if $B175
                  local.get $l42
                  local.get $l26
                  i32.const 44
                  i32.mul
                  i32.add
                  local.tee $p1
                  local.get $l42
                  local.get $l33
                  i32.const 44
                  i32.mul
                  i32.add
                  local.tee $l18
                  f32.load
                  local.get $p1
                  f32.load
                  f32.add
                  f32.store
                  local.get $p1
                  local.get $l18
                  f32.load offset=4
                  local.get $p1
                  f32.load offset=4
                  f32.add
                  f32.store offset=4
                  local.get $p1
                  local.get $l18
                  f32.load offset=8
                  local.get $p1
                  f32.load offset=8
                  f32.add
                  f32.store offset=8
                  local.get $p1
                  local.get $l18
                  f32.load offset=12
                  local.get $p1
                  f32.load offset=12
                  f32.add
                  f32.store offset=12
                  local.get $p1
                  local.get $l18
                  f32.load offset=16
                  local.get $p1
                  f32.load offset=16
                  f32.add
                  f32.store offset=16
                  local.get $p1
                  local.get $l18
                  f32.load offset=20
                  local.get $p1
                  f32.load offset=20
                  f32.add
                  f32.store offset=20
                  local.get $p1
                  local.get $l18
                  f32.load offset=24
                  local.get $p1
                  f32.load offset=24
                  f32.add
                  f32.store offset=24
                  local.get $p1
                  local.get $l18
                  f32.load offset=28
                  local.get $p1
                  f32.load offset=28
                  f32.add
                  f32.store offset=28
                  local.get $p1
                  local.get $l18
                  f32.load offset=32
                  local.get $p1
                  f32.load offset=32
                  f32.add
                  f32.store offset=32
                  local.get $p1
                  local.get $l18
                  f32.load offset=36
                  local.get $p1
                  f32.load offset=36
                  f32.add
                  f32.store offset=36
                  local.get $p1
                  local.get $l18
                  f32.load offset=40
                  local.get $p1
                  f32.load offset=40
                  f32.add
                  f32.store offset=40
                  local.get $l39
                  i32.eqz
                  br_if $B175
                  local.get $l39
                  local.get $l26
                  i32.const 4
                  i32.shl
                  i32.add
                  local.tee $p1
                  local.get $l39
                  local.get $l33
                  i32.const 4
                  i32.shl
                  i32.add
                  local.tee $l18
                  f32.load
                  local.get $p1
                  f32.load
                  f32.add
                  f32.store
                  local.get $p1
                  local.get $l18
                  f32.load offset=4
                  local.get $p1
                  f32.load offset=4
                  f32.add
                  f32.store offset=4
                  local.get $p1
                  local.get $l18
                  f32.load offset=8
                  local.get $p1
                  f32.load offset=8
                  f32.add
                  f32.store offset=8
                  local.get $p1
                  local.get $l18
                  f32.load offset=12
                  local.get $p1
                  f32.load offset=12
                  f32.add
                  f32.store offset=12
                end
                local.get $l32
                i32.eqz
                br_if $B174
                local.get $l34
                local.get $l19
                i32.const 44
                i32.mul
                i32.add
                local.tee $p1
                local.get $l34
                local.get $l33
                i32.const 44
                i32.mul
                local.tee $l38
                i32.add
                local.tee $l18
                f32.load
                local.get $p1
                f32.load
                f32.add
                f32.store
                local.get $p1
                local.get $l18
                f32.load offset=4
                local.get $p1
                f32.load offset=4
                f32.add
                f32.store offset=4
                local.get $p1
                local.get $l18
                f32.load offset=8
                local.get $p1
                f32.load offset=8
                f32.add
                f32.store offset=8
                local.get $p1
                local.get $l18
                f32.load offset=12
                local.get $p1
                f32.load offset=12
                f32.add
                f32.store offset=12
                local.get $p1
                local.get $l18
                f32.load offset=16
                local.get $p1
                f32.load offset=16
                f32.add
                f32.store offset=16
                local.get $p1
                local.get $l18
                f32.load offset=20
                local.get $p1
                f32.load offset=20
                f32.add
                f32.store offset=20
                local.get $p1
                local.get $l18
                f32.load offset=24
                local.get $p1
                f32.load offset=24
                f32.add
                f32.store offset=24
                local.get $p1
                local.get $l18
                f32.load offset=28
                local.get $p1
                f32.load offset=28
                f32.add
                f32.store offset=28
                local.get $p1
                local.get $l18
                f32.load offset=32
                local.get $p1
                f32.load offset=32
                f32.add
                f32.store offset=32
                local.get $p1
                local.get $l18
                f32.load offset=36
                local.get $p1
                f32.load offset=36
                f32.add
                f32.store offset=36
                local.get $p1
                local.get $l18
                f32.load offset=40
                local.get $p1
                f32.load offset=40
                f32.add
                f32.store offset=40
                local.get $l81
                local.get $l19
                i32.mul
                local.set $l25
                local.get $l43
                local.set $l18
                local.get $l32
                local.set $l20
                loop $L176
                  local.get $l18
                  local.get $l25
                  i32.add
                  local.tee $p1
                  local.get $l18
                  local.get $l23
                  i32.add
                  local.tee $l19
                  f32.load
                  local.get $p1
                  f32.load
                  f32.add
                  f32.store
                  local.get $p1
                  i32.const 4
                  i32.add
                  local.tee $l22
                  local.get $l19
                  i32.const 4
                  i32.add
                  f32.load
                  local.get $l22
                  f32.load
                  f32.add
                  f32.store
                  local.get $p1
                  i32.const 8
                  i32.add
                  local.tee $l22
                  local.get $l19
                  i32.const 8
                  i32.add
                  f32.load
                  local.get $l22
                  f32.load
                  f32.add
                  f32.store
                  local.get $p1
                  i32.const 12
                  i32.add
                  local.tee $p1
                  local.get $l19
                  i32.const 12
                  i32.add
                  f32.load
                  local.get $p1
                  f32.load
                  f32.add
                  f32.store
                  local.get $l18
                  i32.const 16
                  i32.add
                  local.set $l18
                  local.get $l20
                  i32.const -1
                  i32.add
                  local.tee $l20
                  br_if $L176
                end
                local.get $l35
                br_if $B174
                f32.const 0x0p+0 (;=0;)
                f32.const 0x1p+0 (;=1;)
                local.get $l42
                local.get $l38
                i32.add
                local.tee $p1
                f32.load offset=40
                local.tee $l45
                f32.div
                local.get $l45
                f32.const 0x0p+0 (;=0;)
                f32.eq
                select
                local.get $p1
                f32.load offset=8
                local.get $l36
                local.get $l26
                i32.const 12
                i32.mul
                i32.add
                local.tee $l18
                f32.load offset=8
                local.tee $l45
                f32.mul
                local.get $p1
                f32.load offset=16
                local.get $l18
                f32.load
                local.tee $l47
                f32.mul
                local.get $p1
                f32.load offset=32
                f32.add
                local.tee $l48
                local.get $l48
                f32.add
                f32.add
                local.get $l45
                f32.mul
                local.get $p1
                f32.load offset=4
                local.get $l18
                f32.load offset=4
                local.tee $l48
                f32.mul
                local.get $p1
                f32.load offset=20
                local.get $l45
                f32.mul
                local.get $p1
                f32.load offset=28
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l48
                f32.mul
                local.get $p1
                f32.load
                local.get $l47
                f32.mul
                local.get $p1
                f32.load offset=12
                local.get $l48
                f32.mul
                local.get $p1
                f32.load offset=24
                f32.add
                local.tee $l45
                local.get $l45
                f32.add
                f32.add
                local.get $l47
                f32.mul
                local.get $p1
                f32.load offset=36
                f32.add
                f32.add
                f32.add
                f32.abs
                f32.mul
                local.tee $l45
                local.get $l57
                local.get $l57
                local.get $l45
                f32.lt
                select
                local.set $l57
              end
              local.get $l23
              local.get $l81
              i32.add
              local.set $l23
              local.get $l33
              i32.const 1
              i32.add
              local.tee $l33
              local.get $p4
              i32.ne
              br_if $L173
            end
            i32.const 0
            local.set $l18
            local.get $l30
            local.set $p1
            loop $L177
              block $B178
                local.get $p1
                i32.load
                local.tee $l19
                i32.const -1
                i32.eq
                br_if $B178
                block $B179
                  local.get $l18
                  local.get $l77
                  local.get $l19
                  i32.const 2
                  i32.shl
                  local.tee $l20
                  i32.add
                  i32.load
                  local.tee $l19
                  i32.ne
                  br_if $B179
                  i32.const -1
                  local.set $l19
                  local.get $l30
                  local.get $l20
                  i32.add
                  i32.load
                  local.tee $l20
                  i32.const -1
                  i32.eq
                  br_if $B179
                  local.get $l77
                  local.get $l20
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.set $l19
                end
                local.get $p1
                local.get $l19
                i32.store
              end
              local.get $p1
              i32.const 4
              i32.add
              local.set $p1
              local.get $p4
              local.get $l18
              i32.const 1
              i32.add
              local.tee $l18
              i32.ne
              br_if $L177
            end
            i32.const 0
            local.set $l18
            local.get $l31
            local.set $p1
            loop $L180
              block $B181
                local.get $p1
                i32.load
                local.tee $l19
                i32.const -1
                i32.eq
                br_if $B181
                block $B182
                  local.get $l18
                  local.get $l77
                  local.get $l19
                  i32.const 2
                  i32.shl
                  local.tee $l20
                  i32.add
                  i32.load
                  local.tee $l19
                  i32.ne
                  br_if $B182
                  i32.const -1
                  local.set $l19
                  local.get $l31
                  local.get $l20
                  i32.add
                  i32.load
                  local.tee $l20
                  i32.const -1
                  i32.eq
                  br_if $B182
                  local.get $l77
                  local.get $l20
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.set $l19
                end
                local.get $p1
                local.get $l19
                i32.store
              end
              local.get $p1
              i32.const 4
              i32.add
              local.set $p1
              local.get $p4
              local.get $l18
              i32.const 1
              i32.add
              local.tee $l18
              i32.ne
              br_if $L180
            end
          end
          local.get $l57
          local.get $l56
          local.get $l32
          select
          local.set $l57
          i32.const 0
          local.set $l18
          local.get $p0
          local.set $p1
          i32.const 0
          local.set $l19
          loop $L183
            block $B184
              local.get $l17
              local.get $l77
              local.get $p1
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l23
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l20
              local.get $l17
              local.get $l77
              local.get $p1
              i32.const 4
              i32.add
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l25
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l22
              i32.eq
              br_if $B184
              local.get $l20
              local.get $l17
              local.get $l77
              local.get $p1
              i32.const 8
              i32.add
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l26
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l33
              i32.eq
              br_if $B184
              local.get $l22
              local.get $l33
              i32.eq
              br_if $B184
              local.get $p0
              local.get $l18
              i32.const 2
              i32.shl
              i32.add
              local.tee $l20
              local.get $l23
              i32.store
              local.get $l20
              i32.const 8
              i32.add
              local.get $l26
              i32.store
              local.get $l20
              i32.const 4
              i32.add
              local.get $l25
              i32.store
              local.get $l18
              i32.const 3
              i32.add
              local.set $l18
            end
            local.get $p1
            i32.const 12
            i32.add
            local.set $p1
            local.get $l19
            i32.const 3
            i32.add
            local.tee $l19
            local.get $p2
            i32.lt_u
            br_if $L183
          end
          block $B185
            block $B186
              local.get $l70
              br_if $B186
              local.get $l18
              local.set $p2
              br $B185
            end
            block $B187
              local.get $l18
              local.get $p11
              i32.gt_u
              br_if $B187
              local.get $l18
              local.set $p2
              br $B185
            end
            block $B188
              local.get $l58
              local.get $l57
              f32.le
              br_if $B188
              local.get $l18
              local.set $p2
              br $B185
            end
            f32.const 0x1.fffffep+127 (;=3.40282e+38;)
            local.set $l58
            i32.const 0
            local.set $p2
            local.get $p0
            local.set $p1
            i32.const 0
            local.set $l19
            loop $L189
              block $B190
                local.get $l72
                local.get $l73
                local.get $p1
                i32.load
                local.tee $l20
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.const 2
                i32.shl
                i32.add
                f32.load
                local.tee $l45
                local.get $l57
                f32.gt
                i32.eqz
                br_if $B190
                local.get $p1
                i32.const 4
                i32.add
                i64.load align=4
                local.set $l92
                local.get $p0
                local.get $p2
                i32.const 2
                i32.shl
                i32.add
                local.tee $l22
                local.get $l20
                i32.store
                local.get $l22
                i32.const 4
                i32.add
                local.get $l92
                i64.store align=4
                local.get $l45
                local.get $l58
                local.get $l58
                local.get $l45
                f32.gt
                select
                local.set $l58
                local.get $p2
                i32.const 3
                i32.add
                local.set $p2
              end
              local.get $p1
              i32.const 12
              i32.add
              local.set $p1
              local.get $l19
              i32.const 3
              i32.add
              local.tee $l19
              local.get $l18
              i32.lt_u
              br_if $L189
            end
          end
          local.get $p2
          local.get $p11
          i32.gt_u
          br_if $L114
          br $B112
        end
      end
      local.get $l15
      i32.const 148
      i32.add
      local.get $p0
      local.get $p2
      local.get $p4
      local.get $l17
      call $f2
    end
    block $B191
      block $B192
        local.get $p2
        local.get $p11
        i32.gt_u
        br_if $B192
        local.get $p2
        local.set $l25
        br $B191
      end
      block $B193
        local.get $l70
        br_if $B193
        local.get $p2
        local.set $l25
        br $B191
      end
      block $B194
        local.get $l58
        local.get $l59
        f32.le
        br_if $B194
        local.get $p2
        local.set $l25
        br $B191
      end
      i32.const 1
      local.set $l22
      loop $L195
        local.get $l58
        f32.const 0x1.8p+0 (;=1.5;)
        f32.mul
        local.tee $l45
        local.get $l59
        local.get $l45
        local.get $l59
        f32.lt
        select
        local.set $l48
        f32.const 0x0p+0 (;=0;)
        local.set $l45
        block $B196
          local.get $l71
          i32.eqz
          br_if $B196
          local.get $l72
          local.set $p1
          local.get $l71
          local.set $l18
          loop $L197
            local.get $p1
            f32.load
            local.tee $l47
            local.get $l45
            local.get $l47
            local.get $l48
            f32.le
            select
            local.get $l45
            local.get $l47
            local.get $l45
            f32.gt
            select
            local.set $l45
            local.get $p1
            i32.const 4
            i32.add
            local.set $p1
            local.get $l18
            i32.const -1
            i32.add
            local.tee $l18
            br_if $L197
          end
        end
        f32.const 0x1.fffffep+127 (;=3.40282e+38;)
        local.set $l58
        i32.const 0
        local.set $l25
        local.get $p0
        local.set $p1
        i32.const 0
        local.set $l18
        loop $L198
          block $B199
            local.get $l72
            local.get $l73
            local.get $p1
            i32.load
            local.tee $l19
            i32.const 2
            i32.shl
            i32.add
            i32.load
            i32.const 2
            i32.shl
            i32.add
            f32.load
            local.tee $l47
            local.get $l48
            f32.gt
            i32.eqz
            br_if $B199
            local.get $p1
            i32.const 4
            i32.add
            i64.load align=4
            local.set $l92
            local.get $p0
            local.get $l25
            i32.const 2
            i32.shl
            i32.add
            local.tee $l20
            local.get $l19
            i32.store
            local.get $l20
            i32.const 4
            i32.add
            local.get $l92
            i64.store align=4
            local.get $l47
            local.get $l58
            local.get $l58
            local.get $l47
            f32.gt
            select
            local.set $l58
            local.get $l25
            i32.const 3
            i32.add
            local.set $l25
          end
          local.get $p1
          i32.const 12
          i32.add
          local.set $p1
          local.get $l18
          i32.const 3
          i32.add
          local.tee $l18
          local.get $p2
          i32.lt_u
          br_if $L198
        end
        block $B200
          local.get $l22
          local.get $l25
          local.get $p2
          i32.ne
          i32.or
          i32.const 1
          i32.and
          br_if $B200
          local.get $p2
          local.set $l25
          br $B191
        end
        local.get $l45
        local.get $l56
        local.get $l56
        local.get $l45
        f32.lt
        select
        local.set $l56
        local.get $l25
        local.get $p11
        i32.le_u
        br_if $B191
        i32.const 0
        local.set $l22
        local.get $l25
        local.set $p2
        local.get $l58
        local.get $l59
        f32.le
        br_if $L195
      end
    end
    block $B201
      local.get $p13
      i32.const 536870912
      i32.and
      i32.eqz
      br_if $B201
      local.get $l78
      i32.const 0
      local.get $p4
      call $f23
      local.set $l26
      block $B202
        local.get $l25
        i32.eqz
        br_if $B202
        local.get $p0
        local.set $p1
        local.get $l25
        local.set $l18
        loop $L203
          local.get $l26
          local.get $p1
          i32.load
          local.tee $l19
          i32.add
          i32.const 1
          i32.store8
          local.get $l26
          local.get $l17
          local.get $l19
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.add
          i32.const 1
          i32.store8
          local.get $p1
          i32.const 4
          i32.add
          local.set $p1
          local.get $l18
          i32.const -1
          i32.add
          local.tee $l18
          br_if $L203
        end
      end
      local.get $l15
      i32.const 148
      i32.add
      local.get $p0
      local.get $l25
      local.get $p4
      local.get $l17
      call $f2
      block $B204
        block $B205
          local.get $p4
          i32.eqz
          br_if $B205
          i32.const 0
          local.set $l20
          loop $L206
            block $B207
              local.get $l26
              local.get $l20
              i32.add
              i32.load8_u
              i32.eqz
              br_if $B207
              block $B208
                local.get $l29
                local.get $l20
                i32.add
                i32.load8_u
                br_table $B208 $B207 $B207 $B208 $B207
              end
              block $B209
                local.get $l17
                local.get $l20
                i32.const 2
                i32.shl
                local.tee $l23
                i32.add
                i32.load
                local.tee $p1
                local.get $l20
                i32.eq
                br_if $B209
                local.get $l36
                local.get $l20
                i32.const 12
                i32.mul
                i32.add
                local.tee $l18
                local.get $l36
                local.get $p1
                i32.const 12
                i32.mul
                i32.add
                local.tee $p1
                i32.load offset=8
                i32.store offset=8
                local.get $l18
                local.get $p1
                i64.load align=4
                i64.store align=4
                br $B207
              end
              local.get $l42
              local.get $l20
              i32.const 44
              i32.mul
              i32.add
              local.tee $l22
              f32.load offset=40
              local.tee $l64
              local.get $l64
              f32.const 0x1.a36e2ep-14 (;=0.0001;)
              f32.mul
              local.tee $l45
              f32.add
              local.set $l59
              local.get $l22
              f32.load offset=8
              local.tee $p12
              local.get $l45
              f32.add
              local.set $l44
              local.get $l22
              f32.load offset=4
              local.tee $l93
              local.get $l45
              f32.add
              local.set $l46
              local.get $l22
              f32.load
              local.tee $l94
              local.get $l45
              f32.add
              local.set $l53
              local.get $l22
              f32.load offset=32
              local.tee $l95
              local.get $l45
              local.get $l36
              local.get $l20
              i32.const 12
              i32.mul
              i32.add
              local.tee $l35
              f32.load offset=8
              local.tee $l60
              f32.mul
              f32.sub
              local.set $l51
              local.get $l22
              f32.load offset=28
              local.tee $l96
              local.get $l45
              local.get $l35
              f32.load offset=4
              local.tee $l61
              f32.mul
              f32.sub
              local.set $l55
              local.get $l22
              f32.load offset=24
              local.tee $l97
              local.get $l35
              f32.load
              local.tee $l62
              local.get $l45
              f32.mul
              f32.sub
              local.set $l52
              f32.const 0x0p+0 (;=0;)
              local.set $l63
              local.get $l22
              f32.load offset=20
              local.tee $l98
              f32.const 0x0p+0 (;=0;)
              f32.add
              local.set $l57
              local.get $l22
              f32.load offset=16
              local.tee $l99
              f32.const 0x0p+0 (;=0;)
              f32.add
              local.set $l54
              local.get $l22
              f32.load offset=12
              local.tee $l100
              f32.const 0x0p+0 (;=0;)
              f32.add
              local.set $l58
              block $B210
                block $B211
                  local.get $l32
                  i32.eqz
                  br_if $B211
                  local.get $l20
                  local.set $l19
                  loop $L212
                    f32.const 0x0p+0 (;=0;)
                    local.get $l59
                    local.get $l34
                    local.get $l19
                    i32.const 44
                    i32.mul
                    i32.add
                    local.tee $l18
                    f32.load offset=40
                    local.tee $l45
                    f32.div
                    local.get $l45
                    f32.const 0x0p+0 (;=0;)
                    f32.eq
                    select
                    local.set $l45
                    local.get $l43
                    local.get $l19
                    local.get $l32
                    i32.mul
                    i32.const 4
                    i32.shl
                    i32.add
                    local.set $p1
                    local.get $l18
                    f32.load offset=32
                    local.get $l59
                    f32.mul
                    local.get $l51
                    f32.add
                    local.set $l51
                    local.get $l18
                    f32.load offset=28
                    local.get $l59
                    f32.mul
                    local.get $l55
                    f32.add
                    local.set $l55
                    local.get $l18
                    f32.load offset=24
                    local.get $l59
                    f32.mul
                    local.get $l52
                    f32.add
                    local.set $l52
                    local.get $l18
                    f32.load offset=20
                    local.get $l59
                    f32.mul
                    local.get $l57
                    f32.add
                    local.set $l57
                    local.get $l18
                    f32.load offset=16
                    local.get $l59
                    f32.mul
                    local.get $l54
                    f32.add
                    local.set $l54
                    local.get $l18
                    f32.load offset=12
                    local.get $l59
                    f32.mul
                    local.get $l58
                    f32.add
                    local.set $l58
                    local.get $l18
                    f32.load offset=8
                    local.get $l59
                    f32.mul
                    local.get $l44
                    f32.add
                    local.set $l44
                    local.get $l18
                    f32.load offset=4
                    local.get $l59
                    f32.mul
                    local.get $l46
                    f32.add
                    local.set $l46
                    local.get $l18
                    f32.load
                    local.get $l59
                    f32.mul
                    local.get $l53
                    f32.add
                    local.set $l53
                    local.get $l32
                    local.set $l18
                    loop $L213
                      local.get $l51
                      local.get $p1
                      i32.const 8
                      i32.add
                      f32.load
                      local.tee $l47
                      local.get $p1
                      i32.const 12
                      i32.add
                      f32.load
                      local.tee $l50
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l51
                      local.get $l55
                      local.get $p1
                      i32.const 4
                      i32.add
                      f32.load
                      local.tee $l48
                      local.get $l50
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l55
                      local.get $l57
                      local.get $l48
                      local.get $l47
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l57
                      local.get $l53
                      local.get $p1
                      f32.load
                      local.tee $l49
                      local.get $l49
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l53
                      local.get $l52
                      local.get $l49
                      local.get $l50
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l52
                      local.get $l54
                      local.get $l49
                      local.get $l47
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l54
                      local.get $l58
                      local.get $l49
                      local.get $l48
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l58
                      local.get $l44
                      local.get $l47
                      local.get $l47
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l44
                      local.get $l46
                      local.get $l48
                      local.get $l48
                      f32.mul
                      local.get $l45
                      f32.mul
                      f32.sub
                      local.set $l46
                      local.get $p1
                      i32.const 16
                      i32.add
                      local.set $p1
                      local.get $l18
                      i32.const -1
                      i32.add
                      local.tee $l18
                      br_if $L213
                    end
                    local.get $l21
                    local.get $l19
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.tee $l19
                    local.get $l20
                    i32.ne
                    br_if $L212
                  end
                  local.get $l39
                  i32.eqz
                  br_if $B211
                  local.get $l39
                  local.get $l20
                  i32.const 4
                  i32.shl
                  i32.add
                  local.tee $p1
                  f32.load offset=12
                  local.set $l69
                  local.get $p1
                  f32.load offset=8
                  local.set $l65
                  local.get $p1
                  f32.load offset=4
                  local.set $l67
                  local.get $p1
                  f32.load
                  local.set $l45
                  br $B210
                end
                f32.const 0x0p+0 (;=0;)
                local.set $l69
                f32.const 0x0p+0 (;=0;)
                local.set $l65
                f32.const 0x0p+0 (;=0;)
                local.set $l67
                f32.const 0x0p+0 (;=0;)
                local.set $l45
              end
              local.get $l54
              local.get $l53
              f32.div
              local.tee $l48
              local.get $l52
              f32.mul
              local.get $l51
              f32.sub
              local.get $l57
              local.get $l54
              local.get $l58
              local.get $l53
              f32.div
              local.tee $l47
              f32.mul
              f32.sub
              local.tee $l51
              local.get $l46
              local.get $l58
              local.get $l47
              f32.mul
              f32.sub
              local.tee $l50
              f32.div
              local.tee $l49
              local.get $l47
              local.get $l52
              f32.mul
              local.get $l55
              f32.sub
              local.tee $l46
              f32.mul
              f32.sub
              local.set $l55
              local.get $l65
              local.get $l45
              local.get $l48
              f32.mul
              f32.sub
              local.get $l67
              local.get $l45
              local.get $l47
              f32.mul
              f32.sub
              local.tee $l58
              local.get $l49
              f32.mul
              f32.sub
              local.tee $l57
              local.get $l44
              local.get $l54
              local.get $l48
              f32.mul
              f32.sub
              local.get $l51
              local.get $l49
              f32.mul
              f32.sub
              local.tee $l54
              f32.div
              local.tee $l44
              f32.neg
              local.set $l51
              local.get $l58
              local.get $l50
              f32.div
              local.tee $l65
              f32.neg
              local.set $l67
              block $B214
                f32.const 0x0p+0 (;=0;)
                local.get $l45
                local.get $l45
                local.get $l53
                f32.div
                local.tee $l68
                f32.mul
                f32.sub
                local.get $l58
                local.get $l65
                f32.mul
                f32.sub
                local.get $l57
                local.get $l44
                f32.mul
                f32.sub
                local.tee $l58
                f32.abs
                local.get $l59
                f32.const 0x1.0c6f7ap-20 (;=1e-06;)
                f32.mul
                local.tee $l45
                f32.gt
                i32.eqz
                br_if $B214
                local.get $l51
                local.get $l55
                f32.mul
                local.get $l67
                local.get $l46
                f32.mul
                local.get $l68
                local.get $l52
                f32.mul
                local.get $l69
                f32.sub
                f32.add
                f32.add
                local.get $l58
                f32.div
                local.set $l63
              end
              local.get $l53
              f32.abs
              local.get $l45
              f32.gt
              i32.eqz
              br_if $B207
              local.get $l50
              f32.abs
              local.get $l45
              f32.gt
              i32.eqz
              br_if $B207
              local.get $l54
              f32.abs
              local.get $l45
              f32.gt
              i32.eqz
              br_if $B207
              local.get $l68
              f32.neg
              local.get $l63
              f32.mul
              local.get $l48
              f32.neg
              local.get $l51
              local.get $l63
              f32.mul
              local.get $l55
              local.get $l54
              f32.div
              f32.add
              local.tee $l54
              f32.mul
              local.get $l47
              f32.neg
              local.get $l67
              local.get $l63
              f32.mul
              local.get $l49
              f32.neg
              local.get $l54
              f32.mul
              local.get $l46
              local.get $l50
              f32.div
              f32.add
              f32.add
              local.tee $l57
              f32.mul
              local.get $l52
              f32.neg
              local.get $l53
              f32.div
              f32.add
              f32.add
              f32.add
              local.set $l59
              local.get $l24
              local.get $l23
              i32.add
              local.tee $p1
              i32.const 4
              i32.add
              i32.load
              local.tee $l18
              local.get $p1
              i32.load
              local.tee $p1
              i32.sub
              local.set $l33
              local.get $l27
              local.get $p1
              i32.const 3
              i32.shl
              i32.add
              local.set $l23
              f32.const 0x0p+0 (;=0;)
              local.set $l45
              block $B215
                local.get $l18
                local.get $p1
                i32.eq
                local.tee $l38
                br_if $B215
                f32.const 0x0p+0 (;=0;)
                local.set $l45
                local.get $l23
                local.set $p1
                local.get $l33
                local.set $l19
                loop $L216
                  local.get $l36
                  local.get $p1
                  i32.const 4
                  i32.add
                  i32.load
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $l18
                  f32.load offset=8
                  local.get $l60
                  f32.sub
                  local.tee $l47
                  local.get $l47
                  f32.mul
                  local.get $l18
                  f32.load
                  local.get $l62
                  f32.sub
                  local.tee $l47
                  local.get $l47
                  f32.mul
                  local.get $l18
                  f32.load offset=4
                  local.get $l61
                  f32.sub
                  local.tee $l47
                  local.get $l47
                  f32.mul
                  f32.add
                  f32.add
                  local.tee $l47
                  local.get $l36
                  local.get $p1
                  i32.load
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $l18
                  f32.load offset=8
                  local.get $l60
                  f32.sub
                  local.tee $l48
                  local.get $l48
                  f32.mul
                  local.get $l18
                  f32.load
                  local.get $l62
                  f32.sub
                  local.tee $l48
                  local.get $l48
                  f32.mul
                  local.get $l18
                  f32.load offset=4
                  local.get $l61
                  f32.sub
                  local.tee $l48
                  local.get $l48
                  f32.mul
                  f32.add
                  f32.add
                  local.tee $l48
                  local.get $l45
                  local.get $l45
                  local.get $l48
                  f32.lt
                  select
                  local.tee $l45
                  local.get $l45
                  local.get $l47
                  f32.lt
                  select
                  local.set $l45
                  local.get $p1
                  i32.const 8
                  i32.add
                  local.set $p1
                  local.get $l19
                  i32.const -1
                  i32.add
                  local.tee $l19
                  br_if $L216
                end
                local.get $l45
                f32.sqrt
                local.tee $l45
                local.get $l45
                f32.mul
                local.set $l45
              end
              local.get $l54
              local.get $l60
              f32.sub
              local.tee $l47
              local.get $l47
              f32.mul
              local.get $l59
              local.get $l62
              f32.sub
              local.tee $l47
              local.get $l47
              f32.mul
              local.get $l57
              local.get $l61
              f32.sub
              local.tee $l47
              local.get $l47
              f32.mul
              f32.add
              f32.add
              local.get $l45
              f32.gt
              br_if $B207
              local.get $l22
              f32.load offset=36
              local.set $l63
              block $B217
                local.get $l38
                br_if $B217
                loop $L218
                  local.get $l36
                  local.get $l23
                  i32.const 4
                  i32.add
                  i32.load
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $p1
                  f32.load
                  local.get $l36
                  local.get $l23
                  i32.load
                  i32.const 12
                  i32.mul
                  i32.add
                  local.tee $l18
                  f32.load
                  local.tee $l48
                  f32.sub
                  local.tee $l45
                  local.get $l61
                  local.get $l18
                  f32.load offset=4
                  local.tee $l49
                  f32.sub
                  local.tee $l52
                  f32.mul
                  local.get $p1
                  f32.load offset=4
                  local.get $l49
                  f32.sub
                  local.tee $l47
                  local.get $l62
                  local.get $l48
                  f32.sub
                  local.tee $l58
                  f32.mul
                  f32.sub
                  local.tee $l50
                  local.get $l45
                  local.get $l57
                  local.get $l49
                  f32.sub
                  local.tee $l44
                  f32.mul
                  local.get $l47
                  local.get $l59
                  local.get $l48
                  f32.sub
                  local.tee $l46
                  f32.mul
                  f32.sub
                  local.tee $l49
                  f32.mul
                  local.get $l47
                  local.get $l60
                  local.get $l18
                  f32.load offset=8
                  local.tee $l53
                  f32.sub
                  local.tee $l51
                  f32.mul
                  local.get $p1
                  f32.load offset=8
                  local.get $l53
                  f32.sub
                  local.tee $l48
                  local.get $l52
                  f32.mul
                  f32.sub
                  local.tee $l52
                  local.get $l47
                  local.get $l54
                  local.get $l53
                  f32.sub
                  local.tee $l55
                  f32.mul
                  local.get $l48
                  local.get $l44
                  f32.mul
                  f32.sub
                  local.tee $l47
                  f32.mul
                  local.get $l48
                  local.get $l58
                  f32.mul
                  local.get $l45
                  local.get $l51
                  f32.mul
                  f32.sub
                  local.tee $l53
                  local.get $l48
                  local.get $l46
                  f32.mul
                  local.get $l45
                  local.get $l55
                  f32.mul
                  f32.sub
                  local.tee $l45
                  f32.mul
                  f32.add
                  f32.add
                  local.get $l50
                  local.get $l50
                  f32.mul
                  local.get $l52
                  local.get $l52
                  f32.mul
                  local.get $l53
                  local.get $l53
                  f32.mul
                  f32.add
                  f32.add
                  local.get $l49
                  local.get $l49
                  f32.mul
                  local.get $l47
                  local.get $l47
                  f32.mul
                  local.get $l45
                  local.get $l45
                  f32.mul
                  f32.add
                  f32.add
                  f32.mul
                  f32.sqrt
                  f32.const 0x1p-2 (;=0.25;)
                  f32.mul
                  f32.le
                  br_if $B207
                  local.get $l23
                  i32.const 8
                  i32.add
                  local.set $l23
                  local.get $l33
                  i32.const -1
                  i32.add
                  local.tee $l33
                  br_if $L218
                end
              end
              f32.const 0x0p+0 (;=0;)
              f32.const 0x1p+0 (;=1;)
              local.get $l64
              f32.div
              local.get $l64
              f32.const 0x0p+0 (;=0;)
              f32.eq
              select
              local.tee $l45
              local.get $p12
              local.get $l54
              f32.mul
              local.get $l99
              local.get $l59
              f32.mul
              local.get $l95
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l54
              f32.mul
              local.get $l93
              local.get $l57
              f32.mul
              local.get $l98
              local.get $l54
              f32.mul
              local.get $l96
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l57
              f32.mul
              local.get $l94
              local.get $l59
              f32.mul
              local.get $l100
              local.get $l57
              f32.mul
              local.get $l97
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l59
              f32.mul
              local.get $l63
              f32.add
              f32.add
              f32.add
              f32.abs
              f32.mul
              local.get $l45
              local.get $p12
              local.get $l60
              f32.mul
              local.get $l99
              local.get $l62
              f32.mul
              local.get $l95
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l60
              f32.mul
              local.get $l93
              local.get $l61
              f32.mul
              local.get $l98
              local.get $l60
              f32.mul
              local.get $l96
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l61
              f32.mul
              local.get $l94
              local.get $l62
              f32.mul
              local.get $l100
              local.get $l61
              f32.mul
              local.get $l97
              f32.add
              local.tee $l47
              local.get $l47
              f32.add
              f32.add
              local.get $l62
              f32.mul
              local.get $l63
              f32.add
              f32.add
              f32.add
              f32.abs
              f32.mul
              f32.const 0x1.8p+0 (;=1.5;)
              f32.mul
              f32.const 0x1.0c6f7ap-20 (;=1e-06;)
              f32.add
              f32.gt
              br_if $B207
              local.get $l35
              local.get $l54
              f32.store offset=8
              local.get $l35
              local.get $l57
              f32.store offset=4
              local.get $l35
              local.get $l59
              f32.store
            end
            local.get $l20
            i32.const 1
            i32.add
            local.tee $l20
            local.get $p4
            i32.ne
            br_if $L206
          end
          block $B219
            local.get $l32
            br_if $B219
            i32.const 0
            local.set $l32
            br $B204
          end
          i32.const 0
          local.set $l22
          loop $L220
            block $B221
              local.get $l26
              local.get $l22
              i32.add
              i32.load8_u
              i32.eqz
              br_if $B221
              local.get $l17
              local.get $l22
              i32.const 2
              i32.shl
              local.tee $p1
              i32.add
              i32.load
              local.get $l22
              i32.ne
              br_if $B221
              local.get $l29
              local.get $l22
              i32.add
              local.set $l33
              local.get $l21
              local.get $p1
              i32.add
              local.set $l35
              local.get $l36
              local.get $l22
              i32.const 12
              i32.mul
              i32.add
              local.set $l19
              local.get $l41
              local.get $l22
              local.get $l32
              i32.mul
              i32.const 2
              i32.shl
              i32.add
              local.set $l38
              i32.const 0
              local.set $l27
              loop $L222
                i32.const -1
                local.set $l20
                block $B223
                  local.get $l33
                  i32.load8_u
                  i32.const -3
                  i32.add
                  i32.const 255
                  i32.and
                  i32.const 1
                  i32.gt_u
                  br_if $B223
                  local.get $l22
                  local.set $l20
                  local.get $l35
                  i32.load
                  local.tee $p1
                  local.get $l22
                  i32.eq
                  br_if $B223
                  local.get $l41
                  local.get $l27
                  i32.const 2
                  i32.shl
                  local.tee $l18
                  i32.add
                  local.set $l23
                  local.get $l38
                  local.get $l18
                  i32.add
                  f32.load
                  local.set $l45
                  local.get $l22
                  local.set $l20
                  loop $L224
                    local.get $l20
                    local.set $l18
                    i32.const -1
                    local.set $l20
                    block $B225
                      local.get $l23
                      local.get $p1
                      local.get $l32
                      i32.mul
                      i32.const 2
                      i32.shl
                      i32.add
                      f32.load
                      local.get $l45
                      f32.ne
                      br_if $B225
                      local.get $l18
                      i32.const -1
                      i32.eq
                      br_if $B225
                      local.get $l18
                      local.set $l20
                      local.get $l34
                      local.get $p1
                      i32.const 44
                      i32.mul
                      i32.add
                      f32.load offset=40
                      local.get $l34
                      local.get $l18
                      i32.const 44
                      i32.mul
                      i32.add
                      f32.load offset=40
                      f32.gt
                      i32.eqz
                      br_if $B225
                      local.get $p1
                      local.set $l20
                    end
                    local.get $l21
                    local.get $p1
                    i32.const 2
                    i32.shl
                    i32.add
                    i32.load
                    local.tee $p1
                    local.get $l22
                    i32.ne
                    br_if $L224
                  end
                end
                local.get $l41
                local.get $l27
                i32.const 2
                i32.shl
                i32.add
                local.set $l23
                local.get $l43
                local.get $l27
                i32.const 4
                i32.shl
                i32.add
                local.set $l24
                local.get $l22
                local.set $p1
                loop $L226
                  local.get $l23
                  local.get $p1
                  local.get $l32
                  i32.mul
                  i32.const 2
                  i32.shl
                  i32.add
                  f32.const 0x0p+0 (;=0;)
                  f32.const 0x1p+0 (;=1;)
                  local.get $l34
                  local.get $p1
                  local.get $l20
                  local.get $l20
                  i32.const -1
                  i32.eq
                  select
                  local.tee $l18
                  i32.const 44
                  i32.mul
                  i32.add
                  f32.load offset=40
                  local.tee $l45
                  f32.div
                  local.get $l45
                  f32.const 0x0p+0 (;=0;)
                  f32.eq
                  select
                  local.get $l24
                  local.get $l18
                  local.get $l32
                  i32.mul
                  i32.const 4
                  i32.shl
                  i32.add
                  local.tee $l18
                  f32.load offset=8
                  local.get $l19
                  f32.load offset=8
                  f32.mul
                  local.get $l18
                  f32.load
                  local.get $l19
                  f32.load
                  f32.mul
                  local.get $l18
                  f32.load offset=4
                  local.get $l19
                  f32.load offset=4
                  f32.mul
                  f32.add
                  f32.add
                  local.get $l18
                  f32.load offset=12
                  f32.add
                  f32.mul
                  f32.store
                  local.get $l21
                  local.get $p1
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.tee $p1
                  local.get $l22
                  i32.ne
                  br_if $L226
                end
                local.get $l27
                i32.const 1
                i32.add
                local.tee $l27
                local.get $l32
                i32.ne
                br_if $L222
              end
            end
            local.get $l22
            i32.const 1
            i32.add
            local.tee $l22
            local.get $p4
            i32.ne
            br_if $L220
            br $B204
          end
        end
        local.get $l32
        br_if $B204
        i32.const 0
        local.set $l32
      end
      local.get $p3
      local.get $p5
      local.get $p6
      local.get $p7
      local.get $p8
      local.get $l32
      local.get $p4
      local.get $l36
      local.get $l41
      local.get $l16
      local.get $l15
      local.get $l40
      local.get $l15
      i32.const 136
      i32.add
      local.get $l29
      local.get $l26
      local.get $p10
      call $f7
    end
    block $B227
      local.get $p13
      i32.const 1073741824
      i32.and
      i32.eqz
      br_if $B227
      local.get $l16
      br_if $B227
      local.get $l25
      i32.eqz
      br_if $B227
      local.get $p0
      local.set $p1
      i32.const 0
      local.set $l23
      loop $L228
        local.get $l29
        local.get $p1
        i32.load
        local.tee $l19
        i32.add
        i32.load8_u
        i32.const 28
        i32.shl
        local.set $l22
        local.get $p1
        i32.const 8
        i32.add
        local.tee $l21
        i32.load
        local.set $l17
        i32.const -2147483648
        local.set $l20
        block $B229
          local.get $l30
          local.get $l19
          i32.const 2
          i32.shl
          local.tee $l27
          i32.add
          i32.load
          local.get $p1
          i32.const 4
          i32.add
          local.tee $l24
          i32.load
          local.tee $l18
          i32.eq
          br_if $B229
          i32.const -2147483648
          i32.const 0
          local.get $l31
          local.get $l18
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l19
          i32.eq
          select
          local.set $l20
        end
        local.get $p1
        local.get $l22
        local.get $l20
        i32.or
        local.get $l19
        i32.or
        i32.store
        local.get $l29
        local.get $l18
        i32.add
        i32.load8_u
        i32.const 28
        i32.shl
        local.set $l32
        i32.const -2147483648
        local.set $l20
        i32.const -2147483648
        local.set $l22
        block $B230
          local.get $l30
          local.get $l18
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l17
          i32.eq
          br_if $B230
          i32.const -2147483648
          i32.const 0
          local.get $l31
          local.get $l17
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l18
          i32.eq
          select
          local.set $l22
        end
        local.get $l24
        local.get $l32
        local.get $l22
        i32.or
        local.get $l18
        i32.or
        i32.store
        local.get $l29
        local.get $l17
        i32.add
        i32.load8_u
        i32.const 28
        i32.shl
        local.set $l18
        block $B231
          local.get $l30
          local.get $l17
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l19
          i32.eq
          br_if $B231
          i32.const -2147483648
          i32.const 0
          local.get $l31
          local.get $l27
          i32.add
          i32.load
          local.get $l17
          i32.eq
          select
          local.set $l20
        end
        local.get $l21
        local.get $l18
        local.get $l20
        i32.or
        local.get $l17
        i32.or
        i32.store
        local.get $p1
        i32.const 12
        i32.add
        local.set $p1
        local.get $l23
        i32.const 3
        i32.add
        local.tee $l23
        local.get $l25
        i32.lt_u
        br_if $L228
      end
    end
    block $B232
      local.get $l16
      i32.eqz
      br_if $B232
      local.get $l25
      i32.eqz
      br_if $B232
      local.get $l25
      local.set $p1
      loop $L233
        local.get $p0
        local.get $l16
        local.get $p0
        i32.load
        i32.const 2
        i32.shl
        i32.add
        i32.load
        i32.store
        local.get $p0
        i32.const 4
        i32.add
        local.set $p0
        local.get $p1
        i32.const -1
        i32.add
        local.tee $p1
        br_if $L233
      end
    end
    block $B234
      local.get $p14
      i32.eqz
      br_if $B234
      local.get $p14
      local.get $l79
      local.get $l56
      f32.sqrt
      f32.mul
      f32.store
    end
    block $B235
      local.get $l15
      i32.load offset=252
      local.tee $l17
      i32.eqz
      br_if $B235
      local.get $l17
      i32.const 2
      i32.shl
      local.get $l15
      i32.const 156
      i32.add
      i32.add
      i32.const -4
      i32.add
      local.set $p1
      loop $L236
        local.get $p1
        i32.load
        i32.const 0
        i32.load offset=37596
        call_indirect $T0 (type $t1)
        local.get $p1
        i32.const -4
        i32.add
        local.set $p1
        local.get $l17
        i32.const -1
        i32.add
        local.tee $l17
        br_if $L236
      end
    end
    local.get $l15
    i32.const 10496
    i32.add
    global.set $g0
    local.get $l25)
