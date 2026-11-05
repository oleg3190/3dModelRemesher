(module
  (type $t0 (func (param i32) (result i32)))
  (type $t1 (func (param i32)))
  (type $t2 (func))
  (type $t3 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 f32 i32 i32) (result i32)))
  (type $t4 (func (param i32 i32 i32 i32 i32)))
  (type $t5 (func (param i32 i32 i32 i32 i32 i32 i32)))
  (type $t6 (func (param i32 i32 i32 i32 i32 i32) (result f32)))
  (type $t7 (func (param i32 i32 i32 i32 i32) (result i32)))
  (type $t8 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 f32 i32 i32 i32 i32)))
  (type $t9 (func (param i32 i32 i32 i32 i32 i32 i32 f32 i32 i32) (result i32)))
  (type $t10 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 f32 i32 i32) (result i32)))
  (type $t11 (func (param i32 i32 i32 i32 i32 i32 i32 i32 f32 i32) (result i32)))
  (type $t12 (func (param i32 i32 i32 i32 i32 i32 f32) (result i32)))
  (type $t13 (func (param i32 i32 i32 i32 i32 i32 f32 i32) (result i32)))
  (type $t14 (func (param i32 i32 i32) (result f32)))
  (type $t15 (func (param i32 i32 i32 i32 i32 i32 i32 i32 i32) (result i32)))
  (type $t16 (func (param i32 i32 i32 i32 i32 i32 i32 i32 f32 i32 i32)))
  (type $t17 (func (param i32 i32 i32 i32) (result i32)))
  (type $t18 (func (param i32 i32 i32 i32)))
  (type $t19 (func (param i32 i32 i32) (result i32)))
  (func $__wasm_call_ctors (type $t2)
    call $f18)
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
  (func $f2 (type $t4) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32)
    (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32)
    local.get $p0
    i32.load offset=4
    local.set $l5
    local.get $p0
    i32.load
    i32.const 4
    i32.add
    i32.const 0
    local.get $p3
    i32.const 2
    i32.shl
    call $f23
    local.set $l6
    local.get $p2
    i32.const 3
    i32.div_u
    local.set $l7
    block $B0
      local.get $p2
      i32.eqz
      br_if $B0
      block $B1
        local.get $p4
        i32.eqz
        br_if $B1
        local.get $p1
        local.set $l8
        local.get $p2
        local.set $l9
        loop $L2
          local.get $l6
          local.get $p4
          local.get $l8
          i32.load
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.const 2
          i32.shl
          i32.add
          local.tee $l10
          local.get $l10
          i32.load
          i32.const 1
          i32.add
          i32.store
          local.get $l8
          i32.const 4
          i32.add
          local.set $l8
          local.get $l9
          i32.const -1
          i32.add
          local.tee $l9
          br_if $L2
          br $B0
        end
      end
      local.get $p1
      local.set $l8
      local.get $p2
      local.set $l9
      loop $L3
        local.get $l6
        local.get $l8
        i32.load
        i32.const 2
        i32.shl
        i32.add
        local.tee $l10
        local.get $l10
        i32.load
        i32.const 1
        i32.add
        i32.store
        local.get $l8
        i32.const 4
        i32.add
        local.set $l8
        local.get $l9
        i32.const -1
        i32.add
        local.tee $l9
        br_if $L3
      end
    end
    block $B4
      local.get $p3
      i32.eqz
      br_if $B4
      i32.const 0
      local.set $l9
      local.get $l6
      local.set $l8
      loop $L5
        local.get $l8
        i32.load
        local.set $l10
        local.get $l8
        local.get $l9
        i32.store
        local.get $l8
        i32.const 4
        i32.add
        local.set $l8
        local.get $l10
        local.get $l9
        i32.add
        local.set $l9
        local.get $p3
        i32.const -1
        i32.add
        local.tee $p3
        br_if $L5
      end
    end
    block $B6
      local.get $p2
      i32.const 3
      i32.lt_u
      br_if $B6
      loop $L7
        local.get $p1
        i32.const 8
        i32.add
        i32.load
        local.set $l8
        local.get $p1
        i32.const 4
        i32.add
        i32.load
        local.set $l9
        local.get $p1
        i32.load
        local.set $p3
        block $B8
          local.get $p4
          i32.eqz
          br_if $B8
          local.get $p4
          local.get $l8
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $l8
          local.get $p4
          local.get $l9
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $l9
          local.get $p4
          local.get $p3
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $p3
        end
        local.get $l5
        local.get $l6
        local.get $p3
        i32.const 2
        i32.shl
        i32.add
        local.tee $l10
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $l9
        i32.store
        local.get $l5
        local.get $l10
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $l8
        i32.store offset=4
        local.get $l10
        local.get $l10
        i32.load
        i32.const 1
        i32.add
        i32.store
        local.get $l5
        local.get $l6
        local.get $l9
        i32.const 2
        i32.shl
        i32.add
        local.tee $l10
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $l8
        i32.store
        local.get $l5
        local.get $l10
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $p3
        i32.store offset=4
        local.get $l10
        local.get $l10
        i32.load
        i32.const 1
        i32.add
        i32.store
        local.get $l5
        local.get $l6
        local.get $l8
        i32.const 2
        i32.shl
        i32.add
        local.tee $l8
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $p3
        i32.store
        local.get $l5
        local.get $l8
        i32.load
        i32.const 3
        i32.shl
        i32.add
        local.get $l9
        i32.store offset=4
        local.get $l8
        local.get $l8
        i32.load
        i32.const 1
        i32.add
        i32.store
        local.get $p1
        i32.const 12
        i32.add
        local.set $p1
        local.get $l7
        i32.const -1
        i32.add
        local.tee $l7
        br_if $L7
      end
    end
    local.get $p0
    i32.load
    i32.const 0
    i32.store)
  (func $f3 (type $t5) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32)
    (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 i32) (local $l15 i32) (local $l16 f32)
    local.get $p3
    i32.const 2
    i32.shr_u
    local.get $p3
    i32.add
    local.set $l7
    i32.const 1
    local.set $l8
    loop $L0
      local.get $l8
      local.tee $l9
      i32.const 1
      i32.shl
      local.set $l8
      local.get $l9
      local.get $l7
      i32.lt_u
      br_if $L0
    end
    i32.const -1
    local.get $l9
    i32.const 2
    i32.shl
    local.tee $l7
    local.get $l9
    i32.const 1073741823
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.set $l8
    local.get $p6
    local.get $p6
    i32.load offset=96
    local.tee $l10
    i32.const 1
    i32.add
    i32.store offset=96
    local.get $p6
    local.get $l10
    i32.const 2
    i32.shl
    i32.add
    local.get $l8
    i32.store
    local.get $l8
    i32.const 255
    local.get $l7
    call $f23
    local.set $l11
    block $B1
      local.get $p3
      i32.eqz
      br_if $B1
      local.get $p4
      i32.const 2
      i32.shr_u
      local.set $p4
      local.get $l9
      i32.const -1
      i32.add
      local.set $l12
      i32.const 0
      local.set $l13
      loop $L2
        local.get $l13
        local.set $l9
        block $B3
          local.get $p5
          i32.eqz
          br_if $B3
          local.get $p5
          local.get $l13
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $l9
        end
        i32.const 0
        local.get $p2
        local.get $l9
        local.get $p4
        i32.mul
        i32.const 2
        i32.shl
        i32.add
        local.tee $l9
        i32.load offset=4
        local.tee $l8
        local.get $l8
        i32.const -2147483648
        i32.eq
        select
        local.tee $l8
        i32.const 17
        i32.shr_u
        local.get $l8
        i32.xor
        i32.const 19349663
        i32.mul
        i32.const 0
        local.get $l9
        i32.load
        local.tee $l8
        local.get $l8
        i32.const -2147483648
        i32.eq
        select
        local.tee $l8
        i32.const 17
        i32.shr_u
        local.get $l8
        i32.xor
        i32.const 73856093
        i32.mul
        i32.xor
        i32.const 0
        local.get $l9
        i32.load offset=8
        local.tee $l9
        local.get $l9
        i32.const -2147483648
        i32.eq
        select
        local.tee $l9
        i32.const 17
        i32.shr_u
        local.get $l9
        i32.xor
        i32.const 83492791
        i32.mul
        i32.xor
        local.get $l12
        i32.and
        local.set $l8
        local.get $l13
        i32.const 2
        i32.shl
        local.set $l14
        block $B4
          block $B5
            block $B6
              local.get $p5
              i32.eqz
              br_if $B6
              local.get $l11
              local.get $l8
              i32.const 2
              i32.shl
              i32.add
              local.tee $l7
              i32.load
              local.tee $l9
              i32.const -1
              i32.eq
              br_if $B5
              local.get $p2
              local.get $p5
              local.get $l14
              i32.add
              i32.load
              local.get $p4
              i32.mul
              i32.const 2
              i32.shl
              i32.add
              local.tee $l15
              f32.load
              local.set $l16
              i32.const 1
              local.set $l10
              loop $L7
                local.get $l10
                local.set $l7
                block $B8
                  local.get $p2
                  local.get $p5
                  local.get $l9
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.get $p4
                  i32.mul
                  i32.const 2
                  i32.shl
                  i32.add
                  local.tee $l10
                  f32.load
                  local.get $l16
                  f32.ne
                  br_if $B8
                  local.get $l10
                  f32.load offset=4
                  local.get $l15
                  f32.load offset=4
                  f32.ne
                  br_if $B8
                  local.get $l10
                  f32.load offset=8
                  local.get $l15
                  f32.load offset=8
                  f32.eq
                  br_if $B4
                end
                local.get $l7
                i32.const 1
                i32.add
                local.set $l10
                local.get $l11
                local.get $l8
                local.get $l7
                i32.add
                local.get $l12
                i32.and
                local.tee $l8
                i32.const 2
                i32.shl
                i32.add
                local.tee $l7
                i32.load
                local.tee $l9
                i32.const -1
                i32.ne
                br_if $L7
                br $B5
              end
            end
            local.get $l11
            local.get $l8
            i32.const 2
            i32.shl
            i32.add
            local.tee $l7
            i32.load
            local.tee $l9
            i32.const -1
            i32.eq
            br_if $B5
            local.get $p2
            local.get $l13
            local.get $p4
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $l15
            f32.load
            local.set $l16
            i32.const 1
            local.set $l10
            loop $L9
              local.get $l10
              local.set $l7
              block $B10
                local.get $p2
                local.get $l9
                local.get $p4
                i32.mul
                i32.const 2
                i32.shl
                i32.add
                local.tee $l10
                f32.load
                local.get $l16
                f32.ne
                br_if $B10
                local.get $l10
                f32.load offset=4
                local.get $l15
                f32.load offset=4
                f32.ne
                br_if $B10
                local.get $l10
                f32.load offset=8
                local.get $l15
                f32.load offset=8
                f32.eq
                br_if $B4
              end
              local.get $l7
              i32.const 1
              i32.add
              local.set $l10
              local.get $l11
              local.get $l8
              local.get $l7
              i32.add
              local.get $l12
              i32.and
              local.tee $l8
              i32.const 2
              i32.shl
              i32.add
              local.tee $l7
              i32.load
              local.tee $l9
              i32.const -1
              i32.ne
              br_if $L9
            end
          end
          local.get $l7
          local.get $l13
          i32.store
          local.get $l13
          local.set $l9
        end
        local.get $p0
        local.get $l14
        i32.add
        local.get $l9
        i32.store
        local.get $l13
        i32.const 1
        i32.add
        local.tee $l13
        local.get $p3
        i32.ne
        br_if $L2
      end
    end
    local.get $l11
    i32.const 0
    i32.load offset=37596
    call_indirect $T0 (type $t1)
    local.get $p6
    local.get $p6
    i32.load offset=96
    i32.const -1
    i32.add
    i32.store offset=96
    block $B11
      local.get $p1
      i32.eqz
      br_if $B11
      local.get $p3
      i32.eqz
      br_if $B11
      i32.const 0
      local.set $l9
      local.get $p1
      local.set $l8
      loop $L12
        local.get $l8
        local.get $l9
        i32.store
        local.get $l8
        i32.const 4
        i32.add
        local.set $l8
        local.get $p3
        local.get $l9
        i32.const 1
        i32.add
        local.tee $l9
        i32.ne
        br_if $L12
      end
      i32.const 0
      local.set $l9
      local.get $p1
      local.set $l8
      loop $L13
        block $B14
          local.get $l9
          local.get $p0
          i32.load
          local.tee $l7
          i32.eq
          br_if $B14
          local.get $l8
          local.get $p1
          local.get $l7
          i32.const 2
          i32.shl
          i32.add
          local.tee $l7
          i32.load
          i32.store
          local.get $l7
          local.get $l9
          i32.store
        end
        local.get $p0
        i32.const 4
        i32.add
        local.set $p0
        local.get $l8
        i32.const 4
        i32.add
        local.set $l8
        local.get $p3
        local.get $l9
        i32.const 1
        i32.add
        local.tee $l9
        i32.ne
        br_if $L13
      end
    end)
  (func $f4 (type $t6) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (result f32)
    (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 f32) (local $l12 f32) (local $l13 f32) (local $l14 f32) (local $l15 f32)
    global.get $g0
    i32.const 32
    i32.sub
    local.tee $l6
    i32.const 0
    i32.load offset=36872
    i32.store offset=24
    local.get $l6
    i32.const 0
    i64.load offset=36864 align=4
    i64.store offset=16
    local.get $l6
    i32.const 0
    i32.load offset=36884
    i32.store offset=8
    local.get $l6
    i32.const 0
    i64.load offset=36876 align=4
    i64.store
    block $B0
      local.get $p2
      i32.eqz
      br_if $B0
      local.get $p3
      i32.const 2
      i32.shr_u
      local.set $l7
      block $B1
        local.get $p0
        br_if $B1
        block $B2
          local.get $p4
          i32.eqz
          br_if $B2
          i32.const 0
          local.set $l8
          loop $L3
            local.get $p1
            local.get $p4
            local.get $l8
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.get $l7
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.set $l9
            i32.const 0
            local.set $p3
            loop $L4
              local.get $l6
              i32.const 16
              i32.add
              local.get $p3
              i32.add
              local.tee $l10
              local.get $l9
              local.get $p3
              i32.add
              f32.load
              local.tee $l11
              local.get $l10
              f32.load
              local.tee $l12
              local.get $l12
              local.get $l11
              f32.gt
              select
              f32.store
              local.get $l6
              local.get $p3
              i32.add
              local.tee $l10
              local.get $l11
              local.get $l10
              f32.load
              local.tee $l12
              local.get $l12
              local.get $l11
              f32.lt
              select
              f32.store
              local.get $p3
              i32.const 4
              i32.add
              local.tee $p3
              i32.const 12
              i32.ne
              br_if $L4
            end
            local.get $l8
            i32.const 1
            i32.add
            local.tee $l8
            local.get $p2
            i32.ne
            br_if $L3
            br $B0
          end
        end
        local.get $l7
        i32.const 2
        i32.shl
        local.set $l8
        i32.const 0
        local.set $l9
        loop $L5
          i32.const 0
          local.set $p3
          loop $L6
            local.get $l6
            i32.const 16
            i32.add
            local.get $p3
            i32.add
            local.tee $l10
            local.get $p1
            local.get $p3
            i32.add
            f32.load
            local.tee $l11
            local.get $l10
            f32.load
            local.tee $l12
            local.get $l12
            local.get $l11
            f32.gt
            select
            f32.store
            local.get $l6
            local.get $p3
            i32.add
            local.tee $l10
            local.get $l11
            local.get $l10
            f32.load
            local.tee $l12
            local.get $l12
            local.get $l11
            f32.lt
            select
            f32.store
            local.get $p3
            i32.const 4
            i32.add
            local.tee $p3
            i32.const 12
            i32.ne
            br_if $L6
          end
          local.get $p1
          local.get $l8
          i32.add
          local.set $p1
          local.get $l9
          i32.const 1
          i32.add
          local.tee $l9
          local.get $p2
          i32.ne
          br_if $L5
          br $B0
        end
      end
      block $B7
        local.get $p4
        i32.eqz
        br_if $B7
        i32.const 0
        local.set $l8
        loop $L8
          local.get $p0
          local.get $l8
          i32.const 12
          i32.mul
          i32.add
          local.tee $p3
          local.get $p1
          local.get $p4
          local.get $l8
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.get $l7
          i32.mul
          i32.const 2
          i32.shl
          i32.add
          local.tee $l9
          f32.load
          f32.store
          local.get $p3
          local.get $l9
          f32.load offset=4
          f32.store offset=4
          local.get $p3
          local.get $l9
          f32.load offset=8
          f32.store offset=8
          i32.const 0
          local.set $p3
          loop $L9
            local.get $l6
            i32.const 16
            i32.add
            local.get $p3
            i32.add
            local.tee $l10
            local.get $l9
            local.get $p3
            i32.add
            f32.load
            local.tee $l11
            local.get $l10
            f32.load
            local.tee $l12
            local.get $l12
            local.get $l11
            f32.gt
            select
            f32.store
            local.get $l6
            local.get $p3
            i32.add
            local.tee $l10
            local.get $l11
            local.get $l10
            f32.load
            local.tee $l12
            local.get $l12
            local.get $l11
            f32.lt
            select
            f32.store
            local.get $p3
            i32.const 4
            i32.add
            local.tee $p3
            i32.const 12
            i32.ne
            br_if $L9
          end
          local.get $l8
          i32.const 1
          i32.add
          local.tee $l8
          local.get $p2
          i32.ne
          br_if $L8
          br $B0
        end
      end
      local.get $l7
      i32.const 2
      i32.shl
      local.set $p4
      i32.const 0
      local.set $l8
      local.get $p1
      local.set $l9
      loop $L10
        local.get $p0
        local.get $l8
        i32.const 12
        i32.mul
        i32.add
        local.tee $p3
        local.get $p1
        local.get $l8
        local.get $l7
        i32.mul
        i32.const 2
        i32.shl
        i32.add
        local.tee $l10
        f32.load
        f32.store
        local.get $p3
        local.get $l10
        f32.load offset=4
        f32.store offset=4
        local.get $p3
        local.get $l10
        f32.load offset=8
        f32.store offset=8
        i32.const 0
        local.set $p3
        loop $L11
          local.get $l6
          i32.const 16
          i32.add
          local.get $p3
          i32.add
          local.tee $l10
          local.get $l9
          local.get $p3
          i32.add
          f32.load
          local.tee $l11
          local.get $l10
          f32.load
          local.tee $l12
          local.get $l12
          local.get $l11
          f32.gt
          select
          f32.store
          local.get $l6
          local.get $p3
          i32.add
          local.tee $l10
          local.get $l11
          local.get $l10
          f32.load
          local.tee $l12
          local.get $l12
          local.get $l11
          f32.lt
          select
          f32.store
          local.get $p3
          i32.const 4
          i32.add
          local.tee $p3
          i32.const 12
          i32.ne
          br_if $L11
        end
        local.get $l9
        local.get $p4
        i32.add
        local.set $l9
        local.get $l8
        i32.const 1
        i32.add
        local.tee $l8
        local.get $p2
        i32.ne
        br_if $L10
      end
    end
    f32.const 0x0p+0 (;=0;)
    local.get $l6
    f32.load
    local.get $l6
    f32.load offset=16
    local.tee $l12
    f32.sub
    local.tee $l11
    local.get $l11
    f32.const 0x0p+0 (;=0;)
    f32.lt
    select
    local.tee $l11
    local.get $l6
    f32.load offset=4
    local.get $l6
    f32.load offset=20
    local.tee $l13
    f32.sub
    local.tee $l14
    local.get $l14
    local.get $l11
    f32.lt
    select
    local.tee $l11
    local.get $l6
    f32.load offset=8
    local.get $l6
    f32.load offset=24
    local.tee $l14
    f32.sub
    local.tee $l15
    local.get $l15
    local.get $l11
    f32.lt
    select
    local.set $l15
    block $B12
      local.get $p0
      i32.eqz
      br_if $B12
      local.get $p2
      i32.eqz
      br_if $B12
      f32.const 0x0p+0 (;=0;)
      f32.const 0x1p+0 (;=1;)
      local.get $l15
      f32.div
      local.get $l15
      f32.const 0x0p+0 (;=0;)
      f32.eq
      select
      local.set $l11
      loop $L13
        local.get $p0
        local.get $l11
        local.get $p0
        f32.load
        local.get $l12
        f32.sub
        f32.mul
        f32.store
        local.get $p0
        i32.const 4
        i32.add
        local.tee $l6
        local.get $l11
        local.get $l6
        f32.load
        local.get $l13
        f32.sub
        f32.mul
        f32.store
        local.get $p0
        i32.const 8
        i32.add
        local.tee $l6
        local.get $l11
        local.get $l6
        f32.load
        local.get $l14
        f32.sub
        f32.mul
        f32.store
        local.get $p0
        i32.const 12
        i32.add
        local.set $p0
        local.get $p2
        i32.const -1
        i32.add
        local.tee $p2
        br_if $L13
      end
    end
    block $B14
      local.get $p5
      i32.eqz
      br_if $B14
      local.get $p5
      local.get $l14
      f32.store offset=8
      local.get $p5
      local.get $l13
      f32.store offset=4
      local.get $p5
      local.get $l12
      f32.store
    end
    local.get $l15)
  (func $f5 (type $t7) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (result i32)
    (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32)
    block $B0
      local.get $p1
      i32.eqz
      br_if $B0
      i32.const 0
      local.set $l5
      local.get $p0
      local.set $l6
      loop $L1
        local.get $l6
        local.get $l5
        i32.store
        local.get $l6
        i32.const 4
        i32.add
        local.set $l6
        local.get $p1
        local.get $l5
        i32.const 1
        i32.add
        local.tee $l5
        i32.ne
        br_if $L1
      end
    end
    block $B2
      local.get $p3
      i32.eqz
      br_if $B2
      i32.const 0
      local.set $l7
      loop $L3
        local.get $p2
        local.get $l7
        i32.const 2
        i32.shl
        i32.add
        local.set $l8
        i32.const 0
        local.set $l9
        loop $L4
          local.get $p4
          local.get $l8
          local.get $l9
          i32.const 2
          i32.shl
          local.tee $l5
          i32.load offset=36896
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $l6
          block $B5
            local.get $p4
            local.get $l8
            local.get $l5
            i32.add
            i32.load
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $l10
            local.get $p0
            local.get $l10
            i32.const 2
            i32.shl
            i32.add
            local.tee $l11
            i32.load
            local.tee $l5
            i32.eq
            br_if $B5
            loop $L6
              local.get $l11
              local.get $p0
              local.get $l5
              local.tee $l10
              i32.const 2
              i32.shl
              i32.add
              local.tee $l12
              i32.load
              local.tee $l5
              i32.store
              local.get $l12
              local.set $l11
              local.get $l10
              local.get $l5
              i32.ne
              br_if $L6
            end
          end
          block $B7
            local.get $l6
            local.get $p0
            local.get $l6
            i32.const 2
            i32.shl
            i32.add
            local.tee $l11
            i32.load
            local.tee $l5
            i32.eq
            br_if $B7
            loop $L8
              local.get $l11
              local.get $p0
              local.get $l5
              local.tee $l6
              i32.const 2
              i32.shl
              i32.add
              local.tee $l12
              i32.load
              local.tee $l5
              i32.store
              local.get $l12
              local.set $l11
              local.get $l6
              local.get $l5
              i32.ne
              br_if $L8
            end
          end
          block $B9
            local.get $l10
            local.get $l6
            i32.eq
            br_if $B9
            local.get $p0
            local.get $l10
            local.get $l6
            local.get $l10
            local.get $l6
            i32.gt_u
            select
            i32.const 2
            i32.shl
            i32.add
            local.get $l10
            local.get $l6
            local.get $l10
            local.get $l6
            i32.lt_u
            select
            i32.store
          end
          local.get $l9
          i32.const 1
          i32.add
          local.tee $l9
          i32.const 3
          i32.ne
          br_if $L4
        end
        local.get $l7
        i32.const 3
        i32.add
        local.tee $l7
        local.get $p3
        i32.lt_u
        br_if $L3
      end
    end
    block $B10
      local.get $p1
      br_if $B10
      i32.const 0
      return
    end
    i32.const 0
    local.set $l12
    loop $L11
      block $B12
        local.get $p4
        local.get $l12
        i32.const 2
        i32.shl
        local.tee $l5
        i32.add
        i32.load
        local.get $l12
        i32.ne
        br_if $B12
        local.get $l12
        local.set $l6
        block $B13
          local.get $l12
          local.get $p0
          local.get $l5
          i32.add
          local.tee $l9
          i32.load
          local.tee $l5
          i32.eq
          br_if $B13
          local.get $l9
          local.set $l10
          loop $L14
            local.get $l10
            local.get $p0
            local.get $l5
            local.tee $l6
            i32.const 2
            i32.shl
            i32.add
            local.tee $l11
            i32.load
            local.tee $l5
            i32.store
            local.get $l11
            local.set $l10
            local.get $l6
            local.get $l5
            i32.ne
            br_if $L14
          end
        end
        local.get $l9
        local.get $l6
        i32.store
      end
      local.get $l12
      i32.const 1
      i32.add
      local.tee $l12
      local.get $p1
      i32.ne
      br_if $L11
    end
    i32.const 0
    local.set $l11
    local.get $p0
    local.set $l5
    i32.const 0
    local.set $l6
    loop $L15
      block $B16
        block $B17
          local.get $l6
          local.get $p4
          i32.load
          local.tee $l10
          i32.ne
          br_if $B17
          block $B18
            local.get $l6
            local.get $l5
            i32.load
            local.tee $l10
            i32.ne
            br_if $B18
            local.get $l5
            local.get $l11
            i32.store
            local.get $l11
            i32.const 1
            i32.add
            local.set $l11
            br $B16
          end
          local.get $l5
          local.get $p0
          local.get $l10
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.store
          br $B16
        end
        local.get $l5
        local.get $p0
        local.get $l10
        i32.const 2
        i32.shl
        i32.add
        i32.load
        i32.store
      end
      local.get $p4
      i32.const 4
      i32.add
      local.set $p4
      local.get $l5
      i32.const 4
      i32.add
      local.set $l5
      local.get $p1
      local.get $l6
      i32.const 1
      i32.add
      local.tee $l6
      i32.ne
      br_if $L15
    end
    local.get $l11)
  (func $f6 (type $t4) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32)
    (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 f32) (local $l10 f32) (local $l11 i32)
    local.get $p0
    i32.const 0
    local.get $p1
    i32.const 4
    i32.shl
    call $f23
    local.set $l5
    block $B0
      local.get $p4
      i32.eqz
      br_if $B0
      local.get $p2
      local.set $l6
      local.get $p3
      local.set $l7
      local.get $p4
      local.set $l8
      loop $L1
        local.get $l7
        i32.const 8
        i32.add
        f32.load
        local.set $l9
        local.get $l7
        i32.const 4
        i32.add
        f32.load
        local.set $l10
        local.get $l5
        local.get $l6
        i32.load
        i32.const 4
        i32.shl
        i32.add
        local.tee $l11
        local.get $l7
        f32.load
        local.get $l11
        f32.load
        f32.add
        f32.store
        local.get $l11
        local.get $l10
        local.get $l11
        f32.load offset=4
        f32.add
        f32.store offset=4
        local.get $l11
        local.get $l9
        local.get $l11
        f32.load offset=8
        f32.add
        f32.store offset=8
        local.get $l11
        local.get $l11
        f32.load offset=12
        f32.const 0x1p+0 (;=1;)
        f32.add
        f32.store offset=12
        local.get $l6
        i32.const 4
        i32.add
        local.set $l6
        local.get $l7
        i32.const 12
        i32.add
        local.set $l7
        local.get $l8
        i32.const -1
        i32.add
        local.tee $l8
        br_if $L1
      end
    end
    block $B2
      local.get $p1
      i32.eqz
      br_if $B2
      local.get $l5
      local.set $l11
      local.get $p1
      local.set $l7
      loop $L3
        local.get $l11
        i32.const 12
        i32.add
        local.tee $l6
        f32.load
        local.set $l9
        local.get $l6
        i32.const 0
        i32.store
        local.get $l11
        local.get $l11
        f32.load
        f32.const 0x0p+0 (;=0;)
        f32.const 0x1p+0 (;=1;)
        local.get $l9
        f32.div
        local.get $l9
        f32.const 0x0p+0 (;=0;)
        f32.eq
        select
        local.tee $l9
        f32.mul
        f32.store
        local.get $l11
        i32.const 4
        i32.add
        local.tee $l6
        local.get $l9
        local.get $l6
        f32.load
        f32.mul
        f32.store
        local.get $l11
        i32.const 8
        i32.add
        local.tee $l6
        local.get $l9
        local.get $l6
        f32.load
        f32.mul
        f32.store
        local.get $l11
        i32.const 16
        i32.add
        local.set $l11
        local.get $l7
        i32.const -1
        i32.add
        local.tee $l7
        br_if $L3
      end
    end
    block $B4
      local.get $p4
      i32.eqz
      br_if $B4
      loop $L5
        local.get $l5
        local.get $p2
        i32.load
        i32.const 4
        i32.shl
        i32.add
        local.tee $l11
        local.get $p3
        i32.const 8
        i32.add
        f32.load
        local.get $l11
        f32.load offset=8
        f32.sub
        local.tee $l9
        local.get $l9
        f32.mul
        local.get $p3
        f32.load
        local.get $l11
        f32.load
        f32.sub
        local.tee $l9
        local.get $l9
        f32.mul
        local.get $p3
        i32.const 4
        i32.add
        f32.load
        local.get $l11
        f32.load offset=4
        f32.sub
        local.tee $l9
        local.get $l9
        f32.mul
        f32.add
        f32.add
        local.tee $l9
        local.get $l11
        f32.load offset=12
        local.tee $l10
        local.get $l10
        local.get $l9
        f32.lt
        select
        f32.store offset=12
        local.get $p2
        i32.const 4
        i32.add
        local.set $p2
        local.get $p3
        i32.const 12
        i32.add
        local.set $p3
        local.get $p4
        i32.const -1
        i32.add
        local.tee $p4
        br_if $L5
      end
    end
    block $B6
      local.get $p1
      i32.eqz
      br_if $B6
      local.get $l5
      i32.const 12
      i32.add
      local.set $l11
      loop $L7
        local.get $p0
        local.get $l11
        f32.load
        f32.store
        local.get $l11
        i32.const 16
        i32.add
        local.set $l11
        local.get $p0
        i32.const 4
        i32.add
        local.set $p0
        local.get $p1
        i32.const -1
        i32.add
        local.tee $p1
        br_if $L7
      end
    end)
  (func $f7 (type $t8) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (param $p9 i32) (param $p10 i32) (param $p11 f32) (param $p12 i32) (param $p13 i32) (param $p14 i32) (param $p15 i32)
    (local $l16 i32) (local $l17 i32) (local $l18 i32) (local $l19 i32) (local $l20 i32) (local $l21 i32) (local $l22 i32)
    block $B0
      local.get $p6
      i32.eqz
      br_if $B0
      local.get $p1
      i32.const 2
      i32.shr_u
      local.set $l16
      block $B1
        local.get $p5
        i32.eqz
        br_if $B1
        local.get $p3
        i32.const 2
        i32.shr_u
        local.set $l17
        local.get $p5
        i32.const 2
        i32.shl
        local.set $l18
        i32.const 0
        local.set $l19
        loop $L2
          block $B3
            local.get $p14
            local.get $l19
            i32.add
            i32.load8_u
            i32.eqz
            br_if $B3
            local.get $l19
            local.set $p1
            block $B4
              local.get $p9
              i32.eqz
              br_if $B4
              local.get $p9
              local.get $l19
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.set $p1
            end
            block $B5
              local.get $p15
              i32.eqz
              br_if $B5
              local.get $p15
              local.get $p1
              i32.add
              i32.load8_u
              i32.const 1
              i32.and
              br_if $B3
            end
            block $B6
              local.get $p13
              local.get $l19
              i32.add
              i32.load8_u
              i32.const 5
              i32.eq
              br_if $B6
              local.get $p0
              local.get $p1
              local.get $l16
              i32.mul
              i32.const 2
              i32.shl
              i32.add
              local.tee $p3
              local.get $p7
              local.get $l19
              i32.const 12
              i32.mul
              i32.add
              local.tee $l20
              f32.load
              local.get $p11
              f32.mul
              local.get $p12
              f32.load
              f32.add
              f32.store
              local.get $p3
              local.get $l20
              f32.load offset=4
              local.get $p11
              f32.mul
              local.get $p12
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $p3
              local.get $l20
              f32.load offset=8
              local.get $p11
              f32.mul
              local.get $p12
              f32.load offset=8
              f32.add
              f32.store offset=8
            end
            local.get $p2
            local.get $p1
            local.get $l17
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.set $l21
            local.get $p10
            local.set $p1
            local.get $p8
            local.set $p3
            local.get $p5
            local.set $l20
            loop $L7
              local.get $l21
              local.get $p1
              i32.load
              i32.const 2
              i32.shl
              local.tee $l22
              i32.add
              local.get $p3
              f32.load
              local.get $p4
              local.get $l22
              i32.add
              f32.load
              f32.div
              f32.store
              local.get $p1
              i32.const 4
              i32.add
              local.set $p1
              local.get $p3
              i32.const 4
              i32.add
              local.set $p3
              local.get $l20
              i32.const -1
              i32.add
              local.tee $l20
              br_if $L7
            end
          end
          local.get $p8
          local.get $l18
          i32.add
          local.set $p8
          local.get $l19
          i32.const 1
          i32.add
          local.tee $l19
          local.get $p6
          i32.ne
          br_if $L2
          br $B0
        end
      end
      block $B8
        local.get $p15
        br_if $B8
        i32.const 0
        local.set $p1
        local.get $p9
        local.set $p3
        loop $L9
          block $B10
            local.get $p14
            local.get $p1
            i32.add
            i32.load8_u
            i32.eqz
            br_if $B10
            local.get $p1
            local.set $l20
            block $B11
              local.get $p9
              i32.eqz
              br_if $B11
              local.get $p3
              i32.load
              local.set $l20
            end
            local.get $p13
            local.get $p1
            i32.add
            i32.load8_u
            i32.const 5
            i32.eq
            br_if $B10
            local.get $p0
            local.get $l20
            local.get $l16
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $l20
            local.get $p7
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load
            f32.add
            f32.store
            local.get $l20
            local.get $p7
            i32.const 4
            i32.add
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $l20
            local.get $p7
            i32.const 8
            i32.add
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load offset=8
            f32.add
            f32.store offset=8
          end
          local.get $p3
          i32.const 4
          i32.add
          local.set $p3
          local.get $p7
          i32.const 12
          i32.add
          local.set $p7
          local.get $p6
          local.get $p1
          i32.const 1
          i32.add
          local.tee $p1
          i32.ne
          br_if $L9
          br $B0
        end
      end
      block $B12
        local.get $p9
        i32.eqz
        br_if $B12
        loop $L13
          block $B14
            local.get $p14
            i32.load8_u
            i32.eqz
            br_if $B14
            local.get $p15
            local.get $p9
            i32.load
            local.tee $p1
            i32.add
            i32.load8_u
            i32.const 1
            i32.and
            br_if $B14
            local.get $p13
            i32.load8_u
            i32.const 5
            i32.eq
            br_if $B14
            local.get $p0
            local.get $p1
            local.get $l16
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $p1
            local.get $p7
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load
            f32.add
            f32.store
            local.get $p1
            local.get $p7
            i32.const 4
            i32.add
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $p1
            local.get $p7
            i32.const 8
            i32.add
            f32.load
            local.get $p11
            f32.mul
            local.get $p12
            f32.load offset=8
            f32.add
            f32.store offset=8
          end
          local.get $p14
          i32.const 1
          i32.add
          local.set $p14
          local.get $p9
          i32.const 4
          i32.add
          local.set $p9
          local.get $p13
          i32.const 1
          i32.add
          local.set $p13
          local.get $p7
          i32.const 12
          i32.add
          local.set $p7
          local.get $p6
          i32.const -1
          i32.add
          local.tee $p6
          br_if $L13
          br $B0
        end
      end
      local.get $l16
      i32.const 2
      i32.shl
      local.set $p3
      i32.const 0
      local.set $p1
      loop $L15
        block $B16
          local.get $p14
          local.get $p1
          i32.add
          i32.load8_u
          i32.eqz
          br_if $B16
          local.get $p15
          local.get $p1
          i32.add
          i32.load8_u
          i32.const 1
          i32.and
          br_if $B16
          local.get $p13
          local.get $p1
          i32.add
          i32.load8_u
          i32.const 5
          i32.eq
          br_if $B16
          local.get $p0
          local.get $p7
          f32.load
          local.get $p11
          f32.mul
          local.get $p12
          f32.load
          f32.add
          f32.store
          local.get $p0
          i32.const 4
          i32.add
          local.get $p7
          i32.const 4
          i32.add
          f32.load
          local.get $p11
          f32.mul
          local.get $p12
          f32.load offset=4
          f32.add
          f32.store
          local.get $p0
          i32.const 8
          i32.add
          local.get $p7
          i32.const 8
          i32.add
          f32.load
          local.get $p11
          f32.mul
          local.get $p12
          f32.load offset=8
          f32.add
          f32.store
        end
        local.get $p7
        i32.const 12
        i32.add
        local.set $p7
        local.get $p0
        local.get $p3
        i32.add
        local.set $p0
        local.get $p6
        local.get $p1
        i32.const 1
        i32.add
        local.tee $p1
        i32.ne
        br_if $L15
      end
    end)
  (func $meshopt_simplify (type $t9) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 f32) (param $p8 i32) (param $p9 i32) (result i32)
    local.get $p0
    local.get $p1
    local.get $p2
    local.get $p3
    local.get $p4
    local.get $p5
    i32.const 0
    i32.const 0
    i32.const 0
    i32.const 0
    i32.const 0
    local.get $p6
    local.get $p7
    local.get $p8
    local.get $p9
    call $f1)
  (func $meshopt_simplifyWithAttributes (type $t3) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (param $p9 i32) (param $p10 i32) (param $p11 i32) (param $p12 f32) (param $p13 i32) (param $p14 i32) (result i32)
    local.get $p0
    local.get $p1
    local.get $p2
    local.get $p3
    local.get $p4
    local.get $p5
    local.get $p6
    local.get $p7
    local.get $p8
    local.get $p9
    local.get $p10
    local.get $p11
    local.get $p12
    local.get $p13
    local.get $p14
    call $f1)
  (func $meshopt_simplifyWithUpdate (type $t10) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (param $p9 i32) (param $p10 i32) (param $p11 f32) (param $p12 i32) (param $p13 i32) (result i32)
    local.get $p0
    local.get $p0
    local.get $p1
    local.get $p2
    local.get $p3
    local.get $p4
    local.get $p5
    local.get $p6
    local.get $p7
    local.get $p8
    local.get $p9
    local.get $p10
    local.get $p11
    local.get $p12
    i32.const 536870912
    i32.or
    local.get $p13
    call $f1)
  (func $meshopt_simplifySloppy (type $t11) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 f32) (param $p9 i32) (result i32)
    (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 i32) (local $l15 i32) (local $l16 i32) (local $l17 i32) (local $l18 i32) (local $l19 i32) (local $l20 f32) (local $l21 i32) (local $l22 i32) (local $l23 f32) (local $l24 i32) (local $l25 i32) (local $l26 f32) (local $l27 f32) (local $l28 f32) (local $l29 f32) (local $l30 f32) (local $l31 f32) (local $l32 f32) (local $l33 f32) (local $l34 f32) (local $l35 f32)
    global.get $g0
    i32.const 112
    i32.sub
    local.tee $l10
    global.set $g0
    i32.const 0
    local.set $l11
    local.get $l10
    i32.const 12
    i32.add
    i32.const 0
    i32.const 100
    call $f23
    drop
    local.get $l10
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
    local.tee $l12
    i32.store offset=12
    local.get $l10
    i32.const 1
    i32.store offset=108
    local.get $l12
    local.get $p3
    local.get $p4
    local.get $p5
    i32.const 0
    i32.const 0
    call $f4
    drop
    local.get $l10
    i32.const -1
    local.get $p4
    i32.const 2
    i32.shl
    local.get $p4
    i32.const 1073741823
    i32.gt_u
    select
    local.tee $l13
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $p3
    i32.store offset=16
    local.get $l10
    i32.const 2
    i32.store offset=108
    block $B0
      block $B1
        f32.const 0x1.f3fffep+9 (;=1000;)
        f32.const 0x1p+0 (;=1;)
        local.get $p8
        f32.const 0x1p+0 (;=1;)
        local.get $p8
        f32.const 0x1p+0 (;=1;)
        f32.lt
        select
        f32.div
        local.get $p8
        f32.const 0x1.0624dep-10 (;=0.001;)
        f32.lt
        select
        local.tee $p8
        f32.abs
        f32.const 0x1p+31 (;=2.14748e+09;)
        f32.lt
        i32.eqz
        br_if $B1
        local.get $p8
        i32.trunc_f32_s
        local.set $l14
        br $B0
      end
      i32.const -2147483648
      local.set $l14
    end
    local.get $p2
    i32.const 3
    i32.div_u
    local.set $l15
    local.get $p7
    i32.const 6
    i32.div_u
    local.set $l16
    block $B2
      block $B3
        local.get $p6
        br_if $B3
        local.get $l14
        i32.const 2
        i32.lt_s
        br_if $B2
      end
      block $B4
        local.get $p4
        i32.eqz
        br_if $B4
        local.get $l14
        i32.const -1
        i32.add
        f32.convert_i32_s
        local.set $p8
        block $B5
          local.get $p6
          i32.eqz
          br_if $B5
          i32.const 0
          local.set $p5
          local.get $p3
          local.set $l17
          local.get $l12
          local.set $l18
          loop $L6
            block $B7
              block $B8
                local.get $p6
                local.get $p5
                i32.add
                i32.load8_u
                i32.const 1
                i32.and
                i32.eqz
                br_if $B8
                local.get $p5
                i32.const 1073741824
                i32.or
                local.set $l19
                br $B7
              end
              block $B9
                block $B10
                  local.get $l18
                  i32.const 4
                  i32.add
                  f32.load
                  local.get $p8
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l20
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B10
                  local.get $l20
                  i32.trunc_f32_s
                  local.set $l19
                  br $B9
                end
                i32.const -2147483648
                local.set $l19
              end
              local.get $l19
              i32.const 10
              i32.shl
              local.set $l19
              block $B11
                block $B12
                  local.get $l18
                  i32.const 8
                  i32.add
                  f32.load
                  local.get $p8
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l20
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B12
                  local.get $l20
                  i32.trunc_f32_s
                  local.set $l21
                  br $B11
                end
                i32.const -2147483648
                local.set $l21
              end
              local.get $l19
              local.get $l21
              i32.or
              local.set $l19
              block $B13
                block $B14
                  local.get $l18
                  f32.load
                  local.get $p8
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l20
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B14
                  local.get $l20
                  i32.trunc_f32_s
                  local.set $l21
                  br $B13
                end
                i32.const -2147483648
                local.set $l21
              end
              local.get $l19
              local.get $l21
              i32.const 20
              i32.shl
              i32.or
              local.set $l19
            end
            local.get $l17
            local.get $l19
            i32.store
            local.get $l17
            i32.const 4
            i32.add
            local.set $l17
            local.get $l18
            i32.const 12
            i32.add
            local.set $l18
            local.get $p4
            local.get $p5
            i32.const 1
            i32.add
            local.tee $p5
            i32.ne
            br_if $L6
            br $B4
          end
        end
        local.get $l12
        local.set $p5
        local.get $p3
        local.set $l18
        local.get $p4
        local.set $l17
        loop $L15
          block $B16
            block $B17
              local.get $p5
              f32.load
              local.get $p8
              f32.mul
              f32.const 0x1p-1 (;=0.5;)
              f32.add
              local.tee $l20
              f32.abs
              f32.const 0x1p+31 (;=2.14748e+09;)
              f32.lt
              i32.eqz
              br_if $B17
              local.get $l20
              i32.trunc_f32_s
              local.set $l19
              br $B16
            end
            i32.const -2147483648
            local.set $l19
          end
          local.get $l19
          i32.const 20
          i32.shl
          local.set $l19
          block $B18
            block $B19
              local.get $p5
              i32.const 4
              i32.add
              f32.load
              local.get $p8
              f32.mul
              f32.const 0x1p-1 (;=0.5;)
              f32.add
              local.tee $l20
              f32.abs
              f32.const 0x1p+31 (;=2.14748e+09;)
              f32.lt
              i32.eqz
              br_if $B19
              local.get $l20
              i32.trunc_f32_s
              local.set $l21
              br $B18
            end
            i32.const -2147483648
            local.set $l21
          end
          local.get $l21
          i32.const 10
          i32.shl
          local.get $l19
          i32.or
          local.set $l19
          block $B20
            block $B21
              local.get $p5
              i32.const 8
              i32.add
              f32.load
              local.get $p8
              f32.mul
              f32.const 0x1p-1 (;=0.5;)
              f32.add
              local.tee $l20
              f32.abs
              f32.const 0x1p+31 (;=2.14748e+09;)
              f32.lt
              i32.eqz
              br_if $B21
              local.get $l20
              i32.trunc_f32_s
              local.set $l21
              br $B20
            end
            i32.const -2147483648
            local.set $l21
          end
          local.get $l18
          local.get $l19
          local.get $l21
          i32.or
          i32.store
          local.get $p5
          i32.const 12
          i32.add
          local.set $p5
          local.get $l18
          i32.const 4
          i32.add
          local.set $l18
          local.get $l17
          i32.const -1
          i32.add
          local.tee $l17
          br_if $L15
        end
      end
      local.get $p2
      i32.eqz
      br_if $B2
      i32.const 0
      local.set $l11
      local.get $p1
      local.set $p5
      i32.const 0
      local.set $l18
      loop $L22
        local.get $l11
        local.get $p3
        local.get $p5
        i32.const 4
        i32.add
        i32.load
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l17
        local.get $p3
        local.get $p5
        i32.const 8
        i32.add
        i32.load
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l19
        i32.ne
        local.get $p3
        local.get $p5
        i32.load
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $l21
        local.get $l17
        i32.ne
        local.get $l21
        local.get $l19
        i32.ne
        i32.and
        i32.and
        i32.add
        local.set $l11
        local.get $p5
        i32.const 12
        i32.add
        local.set $p5
        local.get $l18
        i32.const 3
        i32.add
        local.tee $l18
        local.get $p2
        i32.lt_u
        br_if $L22
      end
    end
    local.get $p7
    i32.const 3
    i32.div_u
    local.set $l22
    block $B23
      block $B24
        local.get $l16
        f32.convert_i32_u
        f32.sqrt
        f32.const 0x1p-1 (;=0.5;)
        f32.add
        local.tee $p8
        f32.abs
        f32.const 0x1p+31 (;=2.14748e+09;)
        f32.lt
        i32.eqz
        br_if $B24
        local.get $p8
        i32.trunc_f32_s
        local.set $p5
        br $B23
      end
      i32.const -2147483648
      local.set $p5
    end
    local.get $l22
    f32.convert_i32_u
    local.set $l23
    i32.const 0
    local.set $l24
    i32.const 1025
    local.set $l16
    block $B25
      loop $L26
        local.get $l11
        local.get $l22
        i32.ge_u
        br_if $B25
        local.get $l16
        local.get $l14
        i32.sub
        i32.const 2
        i32.lt_s
        br_if $B25
        local.get $p5
        local.get $l16
        i32.const -1
        i32.add
        local.tee $l18
        local.get $p5
        local.get $l18
        i32.lt_s
        select
        local.get $l14
        i32.const 1
        i32.add
        local.get $p5
        local.get $l14
        i32.gt_s
        select
        local.set $l25
        block $B27
          local.get $p4
          i32.eqz
          br_if $B27
          local.get $l25
          i32.const -1
          i32.add
          f32.convert_i32_s
          local.set $p8
          block $B28
            local.get $p6
            i32.eqz
            br_if $B28
            i32.const 0
            local.set $l18
            local.get $p3
            local.set $l17
            local.get $l12
            local.set $p5
            loop $L29
              block $B30
                block $B31
                  local.get $p6
                  local.get $l18
                  i32.add
                  i32.load8_u
                  i32.const 1
                  i32.and
                  i32.eqz
                  br_if $B31
                  local.get $l18
                  i32.const 1073741824
                  i32.or
                  local.set $l19
                  br $B30
                end
                block $B32
                  block $B33
                    local.get $p5
                    i32.const 4
                    i32.add
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B33
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $l19
                    br $B32
                  end
                  i32.const -2147483648
                  local.set $l19
                end
                local.get $l19
                i32.const 10
                i32.shl
                local.set $l19
                block $B34
                  block $B35
                    local.get $p5
                    i32.const 8
                    i32.add
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B35
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $l21
                    br $B34
                  end
                  i32.const -2147483648
                  local.set $l21
                end
                local.get $l19
                local.get $l21
                i32.or
                local.set $l19
                block $B36
                  block $B37
                    local.get $p5
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B37
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $l21
                    br $B36
                  end
                  i32.const -2147483648
                  local.set $l21
                end
                local.get $l19
                local.get $l21
                i32.const 20
                i32.shl
                i32.or
                local.set $l19
              end
              local.get $l17
              local.get $l19
              i32.store
              local.get $l17
              i32.const 4
              i32.add
              local.set $l17
              local.get $p5
              i32.const 12
              i32.add
              local.set $p5
              local.get $p4
              local.get $l18
              i32.const 1
              i32.add
              local.tee $l18
              i32.ne
              br_if $L29
              br $B27
            end
          end
          local.get $l12
          local.set $p5
          local.get $p3
          local.set $l18
          local.get $p4
          local.set $l17
          loop $L38
            block $B39
              block $B40
                local.get $p5
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B40
                local.get $l20
                i32.trunc_f32_s
                local.set $l19
                br $B39
              end
              i32.const -2147483648
              local.set $l19
            end
            local.get $l19
            i32.const 20
            i32.shl
            local.set $l19
            block $B41
              block $B42
                local.get $p5
                i32.const 4
                i32.add
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B42
                local.get $l20
                i32.trunc_f32_s
                local.set $l21
                br $B41
              end
              i32.const -2147483648
              local.set $l21
            end
            local.get $l21
            i32.const 10
            i32.shl
            local.get $l19
            i32.or
            local.set $l19
            block $B43
              block $B44
                local.get $p5
                i32.const 8
                i32.add
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B44
                local.get $l20
                i32.trunc_f32_s
                local.set $l21
                br $B43
              end
              i32.const -2147483648
              local.set $l21
            end
            local.get $l18
            local.get $l19
            local.get $l21
            i32.or
            i32.store
            local.get $p5
            i32.const 12
            i32.add
            local.set $p5
            local.get $l18
            i32.const 4
            i32.add
            local.set $l18
            local.get $l17
            i32.const -1
            i32.add
            local.tee $l17
            br_if $L38
          end
        end
        i32.const 0
        local.set $l18
        block $B45
          local.get $p2
          i32.eqz
          br_if $B45
          local.get $p1
          local.set $p5
          i32.const 0
          local.set $l17
          loop $L46
            local.get $l18
            local.get $p3
            local.get $p5
            i32.const 4
            i32.add
            i32.load
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $l19
            local.get $p3
            local.get $p5
            i32.const 8
            i32.add
            i32.load
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $l21
            i32.ne
            local.get $p3
            local.get $p5
            i32.load
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $p7
            local.get $l19
            i32.ne
            local.get $p7
            local.get $l21
            i32.ne
            i32.and
            i32.and
            i32.add
            local.set $l18
            local.get $p5
            i32.const 12
            i32.add
            local.set $p5
            local.get $l17
            i32.const 3
            i32.add
            local.tee $l17
            local.get $p2
            i32.lt_u
            br_if $L46
          end
        end
        block $B47
          local.get $l15
          f32.convert_i32_u
          local.tee $l20
          local.get $l23
          f32.sub
          local.get $l25
          f32.convert_i32_s
          local.tee $p8
          local.get $l16
          f32.convert_i32_s
          f32.sub
          local.tee $l26
          f32.mul
          local.get $l11
          f32.convert_i32_u
          local.tee $l27
          local.get $l18
          f32.convert_i32_u
          local.tee $l28
          f32.sub
          f32.mul
          local.get $l27
          local.get $l23
          f32.sub
          local.get $p8
          local.get $l14
          f32.convert_i32_s
          f32.sub
          local.tee $l29
          f32.mul
          local.get $l28
          local.get $l20
          f32.sub
          f32.mul
          f32.add
          local.tee $l30
          f32.const 0x0p+0 (;=0;)
          f32.eq
          br_if $B47
          local.get $l20
          local.get $l27
          f32.sub
          local.get $l29
          local.get $l26
          local.get $l28
          local.get $l23
          f32.sub
          f32.mul
          f32.mul
          f32.mul
          local.get $l30
          f32.div
          local.get $p8
          f32.add
          local.set $p8
        end
        block $B48
          block $B49
            local.get $l18
            local.get $l22
            i32.gt_u
            br_if $B49
            local.get $l18
            local.set $l11
            local.get $l25
            local.set $l14
            br $B48
          end
          local.get $l18
          local.set $l15
          local.get $l25
          local.set $l16
        end
        block $B50
          block $B51
            local.get $l24
            i32.const 4
            i32.gt_u
            br_if $B51
            block $B52
              local.get $p8
              f32.const 0x1p-1 (;=0.5;)
              f32.add
              local.tee $p8
              f32.abs
              f32.const 0x1p+31 (;=2.14748e+09;)
              f32.lt
              i32.eqz
              br_if $B52
              local.get $p8
              i32.trunc_f32_s
              local.set $p5
              br $B50
            end
            i32.const -2147483648
            local.set $p5
            br $B50
          end
          local.get $l14
          local.get $l16
          i32.add
          i32.const 2
          i32.div_s
          local.set $p5
        end
        local.get $l24
        i32.const 1
        i32.add
        local.tee $l24
        i32.const 15
        i32.ne
        br_if $L26
      end
    end
    block $B53
      block $B54
        block $B55
          local.get $l11
          br_if $B55
          f32.const 0x1p+0 (;=1;)
          local.set $p8
          i32.const 0
          local.set $l18
          i32.const 2
          local.set $p5
          local.get $p9
          br_if $B54
          br $B53
        end
        local.get $p4
        i32.const 2
        i32.shr_u
        local.get $p4
        i32.add
        local.set $l17
        i32.const 1
        local.set $l18
        loop $L56
          local.get $l18
          local.tee $p5
          i32.const 1
          i32.shl
          local.set $l18
          local.get $p5
          local.get $l17
          i32.lt_u
          br_if $L56
        end
        local.get $l10
        i32.const -1
        local.get $p5
        i32.const 2
        i32.shl
        local.get $p5
        i32.const 1073741823
        i32.gt_u
        select
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l25
        i32.store offset=20
        local.get $l10
        i32.const 3
        i32.store offset=108
        local.get $l10
        local.get $l13
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l16
        i32.store offset=24
        local.get $l10
        i32.const 4
        i32.store offset=108
        block $B57
          local.get $p4
          i32.eqz
          br_if $B57
          local.get $l14
          i32.const -1
          i32.add
          f32.convert_i32_s
          local.set $p8
          block $B58
            local.get $p6
            i32.eqz
            br_if $B58
            i32.const 0
            local.set $l18
            local.get $p3
            local.set $l19
            local.get $l12
            local.set $l17
            loop $L59
              block $B60
                block $B61
                  local.get $p6
                  local.get $l18
                  i32.add
                  i32.load8_u
                  i32.const 1
                  i32.and
                  i32.eqz
                  br_if $B61
                  local.get $l18
                  i32.const 1073741824
                  i32.or
                  local.set $l21
                  br $B60
                end
                block $B62
                  block $B63
                    local.get $l17
                    i32.const 4
                    i32.add
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B63
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $l21
                    br $B62
                  end
                  i32.const -2147483648
                  local.set $l21
                end
                local.get $l21
                i32.const 10
                i32.shl
                local.set $l21
                block $B64
                  block $B65
                    local.get $l17
                    i32.const 8
                    i32.add
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B65
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $p7
                    br $B64
                  end
                  i32.const -2147483648
                  local.set $p7
                end
                local.get $l21
                local.get $p7
                i32.or
                local.set $l21
                block $B66
                  block $B67
                    local.get $l17
                    f32.load
                    local.get $p8
                    f32.mul
                    f32.const 0x1p-1 (;=0.5;)
                    f32.add
                    local.tee $l20
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B67
                    local.get $l20
                    i32.trunc_f32_s
                    local.set $p7
                    br $B66
                  end
                  i32.const -2147483648
                  local.set $p7
                end
                local.get $l21
                local.get $p7
                i32.const 20
                i32.shl
                i32.or
                local.set $l21
              end
              local.get $l19
              local.get $l21
              i32.store
              local.get $l19
              i32.const 4
              i32.add
              local.set $l19
              local.get $l17
              i32.const 12
              i32.add
              local.set $l17
              local.get $p4
              local.get $l18
              i32.const 1
              i32.add
              local.tee $l18
              i32.ne
              br_if $L59
              br $B57
            end
          end
          local.get $l12
          local.set $l18
          local.get $p3
          local.set $l17
          local.get $p4
          local.set $l19
          loop $L68
            block $B69
              block $B70
                local.get $l18
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B70
                local.get $l20
                i32.trunc_f32_s
                local.set $l21
                br $B69
              end
              i32.const -2147483648
              local.set $l21
            end
            local.get $l21
            i32.const 20
            i32.shl
            local.set $l21
            block $B71
              block $B72
                local.get $l18
                i32.const 4
                i32.add
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B72
                local.get $l20
                i32.trunc_f32_s
                local.set $p7
                br $B71
              end
              i32.const -2147483648
              local.set $p7
            end
            local.get $p7
            i32.const 10
            i32.shl
            local.get $l21
            i32.or
            local.set $l21
            block $B73
              block $B74
                local.get $l18
                i32.const 8
                i32.add
                f32.load
                local.get $p8
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l20
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B74
                local.get $l20
                i32.trunc_f32_s
                local.set $p7
                br $B73
              end
              i32.const -2147483648
              local.set $p7
            end
            local.get $l17
            local.get $l21
            local.get $p7
            i32.or
            i32.store
            local.get $l18
            i32.const 12
            i32.add
            local.set $l18
            local.get $l17
            i32.const 4
            i32.add
            local.set $l17
            local.get $l19
            i32.const -1
            i32.add
            local.tee $l19
            br_if $L68
          end
        end
        local.get $l10
        i32.const -1
        local.get $l25
        local.get $p5
        local.get $l16
        local.get $p3
        local.get $p4
        call $f12
        local.tee $l21
        i32.const 44
        i32.mul
        local.tee $p5
        local.get $l21
        i32.const 97612893
        i32.gt_u
        select
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $p3
        i32.store offset=28
        local.get $l10
        i32.const 5
        i32.store offset=108
        local.get $p3
        i32.const 0
        local.get $p5
        call $f23
        local.set $l18
        block $B75
          local.get $p2
          i32.eqz
          br_if $B75
          i32.const 0
          local.set $p7
          local.get $p1
          local.set $p3
          loop $L76
            local.get $l12
            local.get $p3
            i32.const 4
            i32.add
            i32.load
            local.tee $p6
            i32.const 12
            i32.mul
            i32.add
            local.tee $p5
            f32.load
            local.get $l12
            local.get $p3
            i32.load
            local.tee $l14
            i32.const 12
            i32.mul
            i32.add
            local.tee $l17
            f32.load
            local.tee $l28
            f32.sub
            local.tee $l20
            local.get $l12
            local.get $p3
            i32.const 8
            i32.add
            i32.load
            local.tee $l25
            i32.const 12
            i32.mul
            i32.add
            local.tee $l19
            f32.load offset=4
            local.get $l17
            f32.load offset=4
            local.tee $l26
            f32.sub
            local.tee $p8
            f32.mul
            local.get $p5
            f32.load offset=4
            local.get $l26
            f32.sub
            local.tee $l27
            local.get $l19
            f32.load
            local.get $l28
            f32.sub
            local.tee $l30
            f32.mul
            f32.sub
            local.tee $l23
            local.get $l23
            f32.mul
            local.get $l27
            local.get $l19
            f32.load offset=8
            local.get $l17
            f32.load offset=8
            local.tee $l29
            f32.sub
            local.tee $l31
            f32.mul
            local.get $p5
            f32.load offset=8
            local.get $l29
            f32.sub
            local.tee $l27
            local.get $p8
            f32.mul
            f32.sub
            local.tee $p8
            local.get $p8
            f32.mul
            local.get $l27
            local.get $l30
            f32.mul
            local.get $l20
            local.get $l31
            f32.mul
            f32.sub
            local.tee $l20
            local.get $l20
            f32.mul
            f32.add
            f32.add
            local.tee $l30
            f32.sqrt
            local.set $l27
            f32.const 0x1.8p+1 (;=3;)
            f32.const 0x1p+0 (;=1;)
            local.get $l16
            local.get $l14
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $p5
            local.get $l16
            local.get $p6
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $l19
            i32.eq
            local.get $p5
            local.get $l16
            local.get $l25
            i32.const 2
            i32.shl
            i32.add
            i32.load
            local.tee $p6
            i32.eq
            i32.and
            local.tee $l17
            select
            local.set $l31
            block $B77
              local.get $l30
              f32.const 0x0p+0 (;=0;)
              f32.gt
              i32.eqz
              br_if $B77
              local.get $l23
              local.get $l27
              f32.div
              local.set $l23
              local.get $l20
              local.get $l27
              f32.div
              local.set $l20
              local.get $p8
              local.get $l27
              f32.div
              local.set $p8
            end
            local.get $l18
            local.get $p5
            i32.const 44
            i32.mul
            i32.add
            local.tee $p5
            local.get $p5
            f32.load
            local.get $p8
            local.get $l31
            local.get $l27
            f32.sqrt
            f32.mul
            local.tee $l27
            local.get $p8
            f32.mul
            f32.mul
            local.tee $l31
            f32.add
            f32.store
            local.get $p5
            local.get $l20
            local.get $l27
            local.get $l20
            f32.mul
            local.tee $l32
            f32.mul
            local.tee $l33
            local.get $p5
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $p5
            local.get $l23
            local.get $l27
            local.get $l23
            f32.mul
            local.tee $l30
            f32.mul
            local.tee $l34
            local.get $p5
            f32.load offset=8
            f32.add
            f32.store offset=8
            local.get $p5
            local.get $p8
            local.get $l32
            f32.mul
            local.tee $l32
            local.get $p5
            f32.load offset=12
            f32.add
            f32.store offset=12
            local.get $p5
            local.get $l30
            local.get $p8
            f32.mul
            local.tee $l35
            local.get $p5
            f32.load offset=16
            f32.add
            f32.store offset=16
            local.get $p5
            local.get $l20
            local.get $l30
            f32.mul
            local.tee $l30
            local.get $p5
            f32.load offset=20
            f32.add
            f32.store offset=20
            local.get $p5
            local.get $p8
            local.get $l27
            local.get $l23
            local.get $l29
            f32.mul
            local.get $p8
            local.get $l28
            f32.mul
            local.get $l26
            local.get $l20
            f32.mul
            f32.add
            f32.add
            f32.neg
            local.tee $l26
            f32.mul
            local.tee $l28
            f32.mul
            local.tee $p8
            local.get $p5
            f32.load offset=24
            f32.add
            f32.store offset=24
            local.get $p5
            local.get $l20
            local.get $l28
            f32.mul
            local.tee $l20
            local.get $p5
            f32.load offset=28
            f32.add
            f32.store offset=28
            local.get $p5
            local.get $l23
            local.get $l28
            f32.mul
            local.tee $l23
            local.get $p5
            f32.load offset=32
            f32.add
            f32.store offset=32
            local.get $p5
            local.get $l28
            local.get $l26
            f32.mul
            local.tee $l28
            local.get $p5
            f32.load offset=36
            f32.add
            f32.store offset=36
            local.get $p5
            local.get $l27
            local.get $p5
            f32.load offset=40
            f32.add
            f32.store offset=40
            block $B78
              local.get $l17
              br_if $B78
              local.get $l18
              local.get $l19
              i32.const 44
              i32.mul
              i32.add
              local.tee $p5
              local.get $l31
              local.get $p5
              f32.load
              f32.add
              f32.store
              local.get $p5
              local.get $l33
              local.get $p5
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $p5
              local.get $l34
              local.get $p5
              f32.load offset=8
              f32.add
              f32.store offset=8
              local.get $p5
              local.get $l32
              local.get $p5
              f32.load offset=12
              f32.add
              f32.store offset=12
              local.get $p5
              local.get $l35
              local.get $p5
              f32.load offset=16
              f32.add
              f32.store offset=16
              local.get $p5
              local.get $l30
              local.get $p5
              f32.load offset=20
              f32.add
              f32.store offset=20
              local.get $p5
              local.get $p8
              local.get $p5
              f32.load offset=24
              f32.add
              f32.store offset=24
              local.get $p5
              local.get $l20
              local.get $p5
              f32.load offset=28
              f32.add
              f32.store offset=28
              local.get $p5
              local.get $l23
              local.get $p5
              f32.load offset=32
              f32.add
              f32.store offset=32
              local.get $p5
              local.get $l28
              local.get $p5
              f32.load offset=36
              f32.add
              f32.store offset=36
              local.get $p5
              local.get $l27
              local.get $p5
              f32.load offset=40
              f32.add
              f32.store offset=40
              local.get $l18
              local.get $p6
              i32.const 44
              i32.mul
              i32.add
              local.tee $p5
              local.get $l31
              local.get $p5
              f32.load
              f32.add
              f32.store
              local.get $p5
              local.get $l33
              local.get $p5
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $p5
              local.get $l34
              local.get $p5
              f32.load offset=8
              f32.add
              f32.store offset=8
              local.get $p5
              local.get $l32
              local.get $p5
              f32.load offset=12
              f32.add
              f32.store offset=12
              local.get $p5
              local.get $l35
              local.get $p5
              f32.load offset=16
              f32.add
              f32.store offset=16
              local.get $p5
              local.get $l30
              local.get $p5
              f32.load offset=20
              f32.add
              f32.store offset=20
              local.get $p5
              local.get $p8
              local.get $p5
              f32.load offset=24
              f32.add
              f32.store offset=24
              local.get $p5
              local.get $l20
              local.get $p5
              f32.load offset=28
              f32.add
              f32.store offset=28
              local.get $p5
              local.get $l23
              local.get $p5
              f32.load offset=32
              f32.add
              f32.store offset=32
              local.get $p5
              local.get $l28
              local.get $p5
              f32.load offset=36
              f32.add
              f32.store offset=36
              local.get $p5
              local.get $l27
              local.get $p5
              f32.load offset=40
              f32.add
              f32.store offset=40
            end
            local.get $p3
            i32.const 12
            i32.add
            local.set $p3
            local.get $p7
            i32.const 3
            i32.add
            local.tee $p7
            local.get $p2
            i32.lt_u
            br_if $L76
          end
        end
        i32.const 0
        local.set $l19
        local.get $l10
        i32.const -1
        local.get $l21
        i32.const 2
        i32.shl
        local.tee $p5
        local.get $l21
        i32.const 1073741823
        i32.gt_u
        select
        local.tee $p3
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l17
        i32.store offset=32
        local.get $l10
        i32.const 6
        i32.store offset=108
        local.get $l10
        local.get $p3
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $p3
        i32.store offset=36
        local.get $l10
        i32.const 7
        i32.store offset=108
        local.get $l17
        i32.const 255
        local.get $p5
        call $f23
        local.set $l14
        block $B79
          local.get $p4
          i32.eqz
          br_if $B79
          local.get $l16
          local.set $l17
          loop $L80
            f32.const 0x0p+0 (;=0;)
            f32.const 0x1p+0 (;=1;)
            local.get $l18
            local.get $l17
            i32.load
            local.tee $p7
            i32.const 44
            i32.mul
            i32.add
            local.tee $p5
            f32.load offset=40
            local.tee $p8
            f32.div
            local.get $p8
            f32.const 0x0p+0 (;=0;)
            f32.eq
            select
            local.get $p5
            f32.load offset=8
            local.get $l12
            i32.const 8
            i32.add
            f32.load
            local.tee $p8
            f32.mul
            local.get $p5
            f32.load offset=16
            local.get $l12
            f32.load
            local.tee $l20
            f32.mul
            local.get $p5
            f32.load offset=32
            f32.add
            local.tee $l23
            local.get $l23
            f32.add
            f32.add
            local.get $p8
            f32.mul
            local.get $p5
            f32.load offset=4
            local.get $l12
            i32.const 4
            i32.add
            f32.load
            local.tee $l23
            f32.mul
            local.get $p5
            f32.load offset=20
            local.get $p8
            f32.mul
            local.get $p5
            f32.load offset=28
            f32.add
            local.tee $p8
            local.get $p8
            f32.add
            f32.add
            local.get $l23
            f32.mul
            local.get $p5
            f32.load
            local.get $l20
            f32.mul
            local.get $p5
            f32.load offset=12
            local.get $l23
            f32.mul
            local.get $p5
            f32.load offset=24
            f32.add
            local.tee $p8
            local.get $p8
            f32.add
            f32.add
            local.get $l20
            f32.mul
            local.get $p5
            f32.load offset=36
            f32.add
            f32.add
            f32.add
            f32.abs
            f32.mul
            local.set $p8
            block $B81
              block $B82
                local.get $l14
                local.get $p7
                i32.const 2
                i32.shl
                local.tee $p5
                i32.add
                local.tee $p7
                i32.load
                i32.const -1
                i32.eq
                br_if $B82
                local.get $p3
                local.get $p5
                i32.add
                f32.load
                local.get $p8
                f32.gt
                i32.eqz
                br_if $B81
              end
              local.get $p7
              local.get $l19
              i32.store
              local.get $p3
              local.get $p5
              i32.add
              local.get $p8
              f32.store
            end
            local.get $l17
            i32.const 4
            i32.add
            local.set $l17
            local.get $l12
            i32.const 12
            i32.add
            local.set $l12
            local.get $p4
            local.get $l19
            i32.const 1
            i32.add
            local.tee $l19
            i32.ne
            br_if $L80
          end
        end
        block $B83
          block $B84
            local.get $l21
            br_if $B84
            f32.const 0x0p+0 (;=0;)
            local.set $p8
            br $B83
          end
          f32.const 0x0p+0 (;=0;)
          local.set $p8
          loop $L85
            local.get $p3
            f32.load
            local.tee $l20
            local.get $p8
            local.get $p8
            local.get $l20
            f32.lt
            select
            local.set $p8
            local.get $p3
            i32.const 4
            i32.add
            local.set $p3
            local.get $l21
            i32.const -1
            i32.add
            local.tee $l21
            br_if $L85
          end
          local.get $p8
          f32.sqrt
          local.set $p8
        end
        local.get $l11
        i32.const 2
        i32.shr_u
        local.get $l11
        i32.add
        local.set $l18
        i32.const 1
        local.set $p3
        loop $L86
          local.get $p3
          local.tee $p5
          i32.const 1
          i32.shl
          local.set $p3
          local.get $p5
          local.get $l18
          i32.lt_u
          br_if $L86
        end
        i32.const 0
        local.set $l18
        local.get $l10
        i32.const -1
        local.get $p5
        i32.const 2
        i32.shl
        local.tee $p3
        local.get $p5
        i32.const 1073741823
        i32.gt_u
        select
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l17
        i32.store offset=40
        local.get $l17
        i32.const 255
        local.get $p3
        call $f23
        local.set $l21
        block $B87
          local.get $p2
          i32.eqz
          br_if $B87
          local.get $p5
          i32.const -1
          i32.add
          local.set $p7
          i32.const 0
          local.set $l11
          i32.const 0
          local.set $l12
          loop $L88
            block $B89
              local.get $l16
              local.get $p1
              local.get $l12
              i32.const 2
              i32.shl
              i32.add
              local.tee $p5
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $p3
              local.get $l16
              local.get $p5
              i32.const 4
              i32.add
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $l18
              i32.eq
              br_if $B89
              local.get $p3
              local.get $l16
              local.get $p5
              i32.const 8
              i32.add
              i32.load
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.tee $p5
              i32.eq
              br_if $B89
              local.get $l18
              local.get $p5
              i32.eq
              br_if $B89
              local.get $l14
              local.get $p5
              i32.const 2
              i32.shl
              i32.add
              i32.load
              local.set $l19
              block $B90
                block $B91
                  local.get $l14
                  local.get $l18
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.tee $p5
                  local.get $l14
                  local.get $p3
                  i32.const 2
                  i32.shl
                  i32.add
                  i32.load
                  local.tee $p3
                  i32.ge_u
                  br_if $B91
                  local.get $p5
                  local.get $l19
                  i32.ge_u
                  br_if $B91
                  local.get $l19
                  local.set $p4
                  local.get $p3
                  local.set $p6
                  local.get $p5
                  local.set $l19
                  br $B90
                end
                block $B92
                  local.get $l19
                  local.get $p3
                  i32.ge_u
                  br_if $B92
                  local.get $l19
                  local.get $p5
                  i32.ge_u
                  br_if $B92
                  local.get $p3
                  local.set $p4
                  local.get $p5
                  local.set $p6
                  br $B90
                end
                local.get $p5
                local.set $p4
                local.get $l19
                local.set $p6
                local.get $p3
                local.set $l19
              end
              local.get $p0
              local.get $l11
              i32.const 12
              i32.mul
              i32.add
              local.tee $p5
              local.get $l19
              i32.store
              local.get $p5
              i32.const 8
              i32.add
              local.get $p6
              i32.store
              local.get $p5
              i32.const 4
              i32.add
              local.get $p4
              i32.store
              block $B93
                local.get $l21
                local.get $p6
                i32.const 83492791
                i32.mul
                local.get $p4
                i32.const 19349663
                i32.mul
                i32.xor
                local.get $l19
                i32.const 73856093
                i32.mul
                i32.xor
                local.get $p7
                i32.and
                local.tee $l18
                i32.const 2
                i32.shl
                i32.add
                local.tee $p5
                i32.load
                local.tee $p3
                i32.const -1
                i32.eq
                br_if $B93
                i32.const 1
                local.set $l17
                loop $L94
                  local.get $l17
                  local.set $p5
                  block $B95
                    local.get $p0
                    local.get $p3
                    i32.const 12
                    i32.mul
                    i32.add
                    local.tee $p3
                    i32.load
                    local.get $l19
                    i32.ne
                    br_if $B95
                    local.get $p3
                    i32.load offset=4
                    local.get $p4
                    i32.ne
                    br_if $B95
                    local.get $p3
                    i32.load offset=8
                    local.get $p6
                    i32.eq
                    br_if $B89
                  end
                  local.get $p5
                  i32.const 1
                  i32.add
                  local.set $l17
                  local.get $l21
                  local.get $l18
                  local.get $p5
                  i32.add
                  local.get $p7
                  i32.and
                  local.tee $l18
                  i32.const 2
                  i32.shl
                  i32.add
                  local.tee $p5
                  i32.load
                  local.tee $p3
                  i32.const -1
                  i32.ne
                  br_if $L94
                end
              end
              local.get $p5
              local.get $l11
              i32.store
              local.get $l11
              i32.const 1
              i32.add
              local.set $l11
            end
            local.get $l12
            i32.const 3
            i32.add
            local.tee $l12
            local.get $p2
            i32.lt_u
            br_if $L88
          end
          local.get $l11
          i32.const 3
          i32.mul
          local.set $l18
        end
        i32.const 8
        local.set $p5
        local.get $p9
        i32.eqz
        br_if $B53
      end
      local.get $p9
      local.get $p8
      f32.store
    end
    local.get $p5
    i32.const 2
    i32.shl
    local.set $p5
    local.get $l10
    i32.const 12
    i32.add
    i32.const -4
    i32.add
    local.set $p3
    loop $L96
      local.get $p3
      local.get $p5
      i32.add
      i32.load
      i32.const 0
      i32.load offset=37596
      call_indirect $T0 (type $t1)
      local.get $p5
      i32.const -4
      i32.add
      local.tee $p5
      br_if $L96
    end
    local.get $l10
    i32.const 112
    i32.add
    global.set $g0
    local.get $l18)
  (func $f12 (type $t7) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (result i32)
    (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32)
    local.get $p0
    i32.const 255
    local.get $p1
    i32.const 2
    i32.shl
    call $f23
    local.set $p0
    block $B0
      local.get $p4
      br_if $B0
      i32.const 0
      return
    end
    local.get $p1
    i32.const -1
    i32.add
    local.set $l5
    block $B1
      block $B2
        local.get $p3
        i32.eqz
        br_if $B2
        i32.const 0
        local.set $l6
        i32.const 0
        local.set $l7
        loop $L3
          block $B4
            block $B5
              block $B6
                local.get $p0
                local.get $p3
                local.get $l7
                i32.const 2
                i32.shl
                local.tee $l8
                i32.add
                i32.load
                local.tee $l9
                i32.const 13
                i32.shr_u
                local.get $l9
                i32.xor
                i32.const 1540483477
                i32.mul
                local.tee $p1
                i32.const 15
                i32.shr_u
                local.get $p1
                i32.xor
                local.get $l5
                i32.and
                local.tee $l10
                i32.const 2
                i32.shl
                i32.add
                local.tee $l11
                i32.load
                local.tee $l12
                i32.const -1
                i32.eq
                br_if $B6
                i32.const 1
                local.set $p1
                loop $L7
                  local.get $p3
                  local.get $l12
                  i32.const 2
                  i32.shl
                  local.tee $l12
                  i32.add
                  i32.load
                  local.get $l9
                  i32.eq
                  br_if $B5
                  local.get $l10
                  local.get $p1
                  i32.add
                  local.set $l12
                  local.get $p1
                  i32.const 1
                  i32.add
                  local.set $p1
                  local.get $p0
                  local.get $l12
                  local.get $l5
                  i32.and
                  local.tee $l10
                  i32.const 2
                  i32.shl
                  i32.add
                  local.tee $l11
                  i32.load
                  local.tee $l12
                  i32.const -1
                  i32.ne
                  br_if $L7
                end
              end
              local.get $l11
              local.get $l7
              i32.store
              local.get $l6
              local.set $p1
              local.get $l6
              i32.const 1
              i32.add
              local.set $l6
              br $B4
            end
            local.get $p2
            local.get $l12
            i32.add
            i32.load
            local.set $p1
          end
          local.get $p2
          local.get $l8
          i32.add
          local.get $p1
          i32.store
          local.get $l7
          i32.const 1
          i32.add
          local.tee $l7
          local.get $p4
          i32.ne
          br_if $L3
          br $B1
        end
      end
      i32.const 0
      local.set $l6
      i32.const 0
      local.set $l9
      loop $L8
        block $B9
          local.get $p0
          local.get $l9
          i32.const 13
          i32.shr_u
          local.get $l9
          i32.xor
          i32.const 1540483477
          i32.mul
          local.tee $p1
          i32.const 15
          i32.shr_u
          local.get $p1
          i32.xor
          local.get $l5
          i32.and
          local.tee $l10
          i32.const 2
          i32.shl
          i32.add
          local.tee $l11
          i32.load
          local.tee $l12
          i32.const -1
          i32.eq
          br_if $B9
          local.get $l12
          local.get $l9
          i32.eq
          br_if $B9
          i32.const 1
          local.set $p1
          loop $L10
            local.get $p0
            local.get $l10
            local.get $p1
            i32.add
            local.get $l5
            i32.and
            local.tee $l10
            i32.const 2
            i32.shl
            i32.add
            local.tee $l11
            i32.load
            local.tee $l12
            i32.const -1
            i32.eq
            br_if $B9
            local.get $p1
            i32.const 1
            i32.add
            local.set $p1
            local.get $l12
            local.get $l9
            i32.ne
            br_if $L10
          end
        end
        block $B11
          block $B12
            local.get $l12
            i32.const -1
            i32.ne
            br_if $B12
            local.get $l11
            local.get $l9
            i32.store
            local.get $l6
            local.set $p1
            local.get $l6
            i32.const 1
            i32.add
            local.set $l6
            br $B11
          end
          local.get $p2
          local.get $l12
          i32.const 2
          i32.shl
          i32.add
          i32.load
          local.set $p1
        end
        local.get $p2
        local.get $l9
        i32.const 2
        i32.shl
        i32.add
        local.get $p1
        i32.store
        local.get $l9
        i32.const 1
        i32.add
        local.tee $l9
        local.get $p4
        i32.ne
        br_if $L8
      end
    end
    local.get $l6)
  (func $meshopt_simplifyPrune (type $t12) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 f32) (result i32)
    (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 i64)
    global.get $g0
    i32.const 112
    i32.sub
    local.tee $l7
    global.set $g0
    i32.const 0
    local.set $l8
    local.get $l7
    i32.const 12
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
    local.get $l7
    i32.const -1
    local.get $p4
    i32.const 2
    i32.shl
    local.get $p4
    i32.const 1073741823
    i32.gt_u
    select
    local.tee $l9
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $l10
    i32.store offset=12
    local.get $l7
    i32.const 1
    i32.store offset=108
    local.get $l10
    i32.const 0
    local.get $p3
    local.get $p4
    local.get $p5
    i32.const 0
    local.get $l7
    i32.const 12
    i32.add
    call $f3
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
    local.set $l11
    local.get $l7
    i32.const 12
    i32.add
    local.get $l7
    i32.load offset=108
    local.tee $l12
    i32.const 2
    i32.shl
    i32.add
    local.get $l11
    i32.store
    local.get $l7
    local.get $l12
    i32.const 1
    i32.add
    local.tee $l13
    i32.store offset=108
    local.get $l11
    local.get $p3
    local.get $p4
    local.get $p5
    i32.const 0
    i32.const 0
    call $f4
    drop
    local.get $l7
    i32.const 12
    i32.add
    local.get $l13
    i32.const 2
    i32.shl
    i32.add
    local.get $l9
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $p3
    i32.store
    local.get $l7
    local.get $l12
    i32.const 2
    i32.add
    local.tee $p5
    i32.store offset=108
    local.get $l7
    i32.const 12
    i32.add
    local.get $p5
    i32.const 2
    i32.shl
    i32.add
    i32.const -1
    local.get $p3
    local.get $p4
    local.get $p1
    local.get $p2
    local.get $l10
    call $f5
    local.tee $p1
    i32.const 4
    i32.shl
    local.get $p1
    i32.const 805306368
    i32.and
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $l10
    i32.store
    local.get $l10
    local.get $p1
    local.get $p3
    local.get $l11
    local.get $p4
    call $f6
    local.get $l12
    i32.const 3
    i32.add
    local.set $l11
    block $B1
      local.get $p2
      i32.eqz
      br_if $B1
      local.get $p6
      local.get $p6
      f32.mul
      local.set $p6
      i32.const 0
      local.set $l8
      local.get $p0
      local.set $p4
      i32.const 0
      local.set $p1
      loop $L2
        block $B3
          local.get $l10
          local.get $p3
          local.get $p4
          i32.load
          local.tee $p5
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.const 2
          i32.shl
          i32.add
          f32.load
          local.get $p6
          f32.gt
          i32.eqz
          br_if $B3
          local.get $p4
          i32.const 4
          i32.add
          i64.load align=4
          local.set $l14
          local.get $p0
          local.get $l8
          i32.const 2
          i32.shl
          i32.add
          local.tee $l9
          local.get $p5
          i32.store
          local.get $l9
          i32.const 4
          i32.add
          local.get $l14
          i64.store align=4
          local.get $l8
          i32.const 3
          i32.add
          local.set $l8
        end
        local.get $p4
        i32.const 12
        i32.add
        local.set $p4
        local.get $p1
        i32.const 3
        i32.add
        local.tee $p1
        local.get $p2
        i32.lt_u
        br_if $L2
      end
    end
    block $B4
      local.get $l11
      i32.eqz
      br_if $B4
      local.get $l12
      i32.const 2
      i32.shl
      local.get $l7
      i32.const 12
      i32.add
      i32.add
      i32.const 8
      i32.add
      local.set $p4
      loop $L5
        local.get $p4
        i32.load
        i32.const 0
        i32.load offset=37596
        call_indirect $T0 (type $t1)
        local.get $p4
        i32.const -4
        i32.add
        local.set $p4
        local.get $l11
        i32.const -1
        i32.add
        local.tee $l11
        br_if $L5
      end
    end
    local.get $l7
    i32.const 112
    i32.add
    global.set $g0
    local.get $l8)
  (func $meshopt_simplifyPoints (type $t13) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 f32) (param $p7 i32) (result i32)
    (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 i32) (local $l15 i32) (local $l16 i32) (local $l17 f32) (local $l18 f32) (local $l19 i32) (local $l20 i32) (local $l21 i32) (local $l22 i32) (local $l23 i32) (local $l24 i32) (local $l25 f32) (local $l26 i32) (local $l27 i32) (local $l28 i32) (local $l29 i32) (local $l30 i32) (local $l31 f32) (local $l32 f32) (local $l33 f32) (local $l34 f32) (local $l35 f32)
    global.get $g0
    i32.const 112
    i32.sub
    local.tee $l8
    global.set $g0
    block $B0
      block $B1
        local.get $p7
        br_if $B1
        i32.const 0
        local.set $l9
        br $B0
      end
      local.get $l8
      i32.const 12
      i32.add
      i32.const 0
      i32.const 100
      call $f23
      drop
      local.get $l8
      i32.const -1
      local.get $p2
      i32.const 12
      i32.mul
      local.get $p2
      i32.const 357913941
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l10
      i32.store offset=12
      local.get $l8
      i32.const 1
      i32.store offset=108
      local.get $l10
      local.get $p1
      local.get $p2
      local.get $p3
      i32.const 0
      i32.const 0
      call $f4
      drop
      local.get $l8
      i32.const -1
      local.get $p2
      i32.const 2
      i32.shl
      local.get $p2
      i32.const 1073741823
      i32.gt_u
      select
      local.tee $l11
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l12
      i32.store offset=16
      local.get $l8
      i32.const 2
      i32.store offset=108
      local.get $p2
      i32.const 2
      i32.shr_u
      local.get $p2
      i32.add
      local.set $p1
      i32.const 1
      local.set $p3
      loop $L2
        local.get $p3
        local.tee $l13
        i32.const 1
        i32.shl
        local.set $p3
        local.get $l13
        local.get $p1
        i32.lt_u
        br_if $L2
      end
      i32.const 0
      local.set $l14
      local.get $l8
      i32.const -1
      local.get $l13
      i32.const 2
      i32.shl
      local.tee $l15
      local.get $l13
      i32.const 1073741823
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l16
      i32.store offset=20
      local.get $l8
      i32.const 3
      i32.store offset=108
      block $B3
        block $B4
          local.get $p7
          f32.convert_i32_u
          local.tee $l17
          f32.sqrt
          f32.const 0x1p-1 (;=0.5;)
          f32.add
          local.tee $l18
          f32.abs
          f32.const 0x1p+31 (;=2.14748e+09;)
          f32.lt
          i32.eqz
          br_if $B4
          local.get $l18
          i32.trunc_f32_s
          local.set $p3
          br $B3
        end
        i32.const -2147483648
        local.set $p3
      end
      local.get $l13
      i32.const -1
      i32.add
      local.set $l19
      i32.const 1025
      local.set $l20
      i32.const 0
      local.set $l21
      local.get $p2
      local.set $l22
      i32.const 0
      local.set $l23
      loop $L5
        local.get $p3
        local.get $l20
        i32.const -1
        i32.add
        local.tee $p1
        local.get $p3
        local.get $p1
        i32.lt_s
        select
        local.get $l14
        i32.const 1
        i32.add
        local.get $p3
        local.get $l14
        i32.gt_s
        select
        local.set $l9
        block $B6
          block $B7
            local.get $p2
            i32.eqz
            br_if $B7
            local.get $l9
            i32.const -1
            i32.add
            f32.convert_i32_s
            local.set $l18
            local.get $l10
            local.set $p3
            local.get $l12
            local.set $p1
            local.get $p2
            local.set $l24
            loop $L8
              block $B9
                block $B10
                  local.get $p3
                  f32.load
                  local.get $l18
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l25
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B10
                  local.get $l25
                  i32.trunc_f32_s
                  local.set $l26
                  br $B9
                end
                i32.const -2147483648
                local.set $l26
              end
              local.get $l26
              i32.const 20
              i32.shl
              local.set $l26
              block $B11
                block $B12
                  local.get $p3
                  i32.const 4
                  i32.add
                  f32.load
                  local.get $l18
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l25
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B12
                  local.get $l25
                  i32.trunc_f32_s
                  local.set $l27
                  br $B11
                end
                i32.const -2147483648
                local.set $l27
              end
              local.get $l27
              i32.const 10
              i32.shl
              local.get $l26
              i32.or
              local.set $l26
              block $B13
                block $B14
                  local.get $p3
                  i32.const 8
                  i32.add
                  f32.load
                  local.get $l18
                  f32.mul
                  f32.const 0x1p-1 (;=0.5;)
                  f32.add
                  local.tee $l25
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B14
                  local.get $l25
                  i32.trunc_f32_s
                  local.set $l27
                  br $B13
                end
                i32.const -2147483648
                local.set $l27
              end
              local.get $p1
              local.get $l26
              local.get $l27
              i32.or
              i32.store
              local.get $p3
              i32.const 12
              i32.add
              local.set $p3
              local.get $p1
              i32.const 4
              i32.add
              local.set $p1
              local.get $l24
              i32.const -1
              i32.add
              local.tee $l24
              br_if $L8
            end
            local.get $l16
            i32.const 255
            local.get $l15
            call $f23
            local.set $l28
            i32.const 0
            local.set $l29
            i32.const 0
            local.set $l30
            loop $L15
              block $B16
                local.get $l28
                local.get $l12
                local.get $l30
                i32.const 2
                i32.shl
                i32.add
                i32.load
                local.tee $l26
                i32.const 13
                i32.shr_u
                local.get $l26
                i32.xor
                i32.const 1540483477
                i32.mul
                local.tee $p3
                i32.const 15
                i32.shr_u
                local.get $p3
                i32.xor
                local.get $l19
                i32.and
                local.tee $l24
                i32.const 2
                i32.shl
                i32.add
                local.tee $l27
                i32.load
                local.tee $p1
                i32.const -1
                i32.eq
                br_if $B16
                local.get $p1
                local.get $l26
                i32.eq
                br_if $B16
                i32.const 1
                local.set $p3
                loop $L17
                  local.get $l28
                  local.get $l24
                  local.get $p3
                  i32.add
                  local.get $l19
                  i32.and
                  local.tee $l24
                  i32.const 2
                  i32.shl
                  i32.add
                  local.tee $l27
                  i32.load
                  local.tee $p1
                  i32.const -1
                  i32.eq
                  br_if $B16
                  local.get $p3
                  i32.const 1
                  i32.add
                  local.set $p3
                  local.get $p1
                  local.get $l26
                  i32.ne
                  br_if $L17
                end
              end
              local.get $l27
              local.get $l26
              i32.store
              local.get $l29
              local.get $p1
              i32.const -1
              i32.eq
              i32.add
              local.set $l29
              local.get $l30
              i32.const 1
              i32.add
              local.tee $l30
              local.get $p2
              i32.ne
              br_if $L15
              br $B6
            end
          end
          local.get $l16
          i32.const 255
          local.get $l15
          call $f23
          drop
          i32.const 0
          local.set $l29
        end
        block $B18
          local.get $l22
          f32.convert_i32_u
          local.tee $l25
          local.get $l17
          f32.sub
          local.get $l9
          f32.convert_i32_s
          local.tee $l18
          local.get $l20
          f32.convert_i32_s
          f32.sub
          local.tee $l31
          f32.mul
          local.get $l21
          f32.convert_i32_u
          local.tee $l32
          local.get $l29
          f32.convert_i32_u
          local.tee $l33
          f32.sub
          f32.mul
          local.get $l32
          local.get $l17
          f32.sub
          local.get $l18
          local.get $l14
          f32.convert_i32_s
          f32.sub
          local.tee $l34
          f32.mul
          local.get $l33
          local.get $l25
          f32.sub
          f32.mul
          f32.add
          local.tee $l35
          f32.const 0x0p+0 (;=0;)
          f32.eq
          br_if $B18
          local.get $l25
          local.get $l32
          f32.sub
          local.get $l34
          local.get $l31
          local.get $l33
          local.get $l17
          f32.sub
          f32.mul
          f32.mul
          f32.mul
          local.get $l35
          f32.div
          local.get $l18
          f32.add
          local.set $l18
        end
        local.get $l14
        local.get $l9
        local.get $l29
        local.get $p7
        i32.gt_u
        local.tee $p1
        select
        local.set $l14
        local.get $l21
        local.get $l29
        local.get $p1
        select
        local.set $l21
        block $B19
          local.get $l29
          local.get $p7
          i32.eq
          br_if $B19
          local.get $l9
          local.get $l20
          local.get $p1
          select
          local.tee $l20
          local.get $l14
          i32.sub
          i32.const 2
          i32.lt_s
          br_if $B19
          block $B20
            block $B21
              local.get $l23
              i32.const 4
              i32.gt_u
              br_if $B21
              block $B22
                local.get $l18
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l18
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B22
                local.get $l18
                i32.trunc_f32_s
                local.set $p3
                br $B20
              end
              i32.const -2147483648
              local.set $p3
              br $B20
            end
            local.get $l14
            local.get $l20
            i32.add
            i32.const 2
            i32.div_s
            local.set $p3
          end
          local.get $l29
          local.get $l22
          local.get $p1
          select
          local.set $l22
          local.get $l23
          i32.const 1
          i32.add
          local.tee $l23
          i32.const 15
          i32.ne
          br_if $L5
        end
      end
      block $B23
        block $B24
          local.get $l21
          br_if $B24
          i32.const 3
          local.set $p3
          i32.const 0
          local.set $l9
          br $B23
        end
        local.get $l8
        local.get $l11
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l26
        i32.store offset=24
        local.get $l8
        i32.const 4
        i32.store offset=108
        local.get $l14
        i32.const -1
        i32.add
        f32.convert_i32_s
        local.set $l25
        block $B25
          local.get $p2
          i32.eqz
          br_if $B25
          local.get $l10
          local.set $p3
          local.get $l12
          local.set $p1
          local.get $p2
          local.set $l24
          loop $L26
            block $B27
              block $B28
                local.get $p3
                f32.load
                local.get $l25
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l18
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B28
                local.get $l18
                i32.trunc_f32_s
                local.set $l27
                br $B27
              end
              i32.const -2147483648
              local.set $l27
            end
            local.get $l27
            i32.const 20
            i32.shl
            local.set $l27
            block $B29
              block $B30
                local.get $p3
                i32.const 4
                i32.add
                f32.load
                local.get $l25
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l18
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B30
                local.get $l18
                i32.trunc_f32_s
                local.set $l28
                br $B29
              end
              i32.const -2147483648
              local.set $l28
            end
            local.get $l28
            i32.const 10
            i32.shl
            local.get $l27
            i32.or
            local.set $l27
            block $B31
              block $B32
                local.get $p3
                i32.const 8
                i32.add
                f32.load
                local.get $l25
                f32.mul
                f32.const 0x1p-1 (;=0.5;)
                f32.add
                local.tee $l18
                f32.abs
                f32.const 0x1p+31 (;=2.14748e+09;)
                f32.lt
                i32.eqz
                br_if $B32
                local.get $l18
                i32.trunc_f32_s
                local.set $l28
                br $B31
              end
              i32.const -2147483648
              local.set $l28
            end
            local.get $p1
            local.get $l27
            local.get $l28
            i32.or
            i32.store
            local.get $p3
            i32.const 12
            i32.add
            local.set $p3
            local.get $p1
            i32.const 4
            i32.add
            local.set $p1
            local.get $l24
            i32.const -1
            i32.add
            local.tee $l24
            br_if $L26
          end
        end
        local.get $l8
        i32.const -1
        local.get $l16
        local.get $l13
        local.get $l26
        local.get $l12
        local.get $p2
        call $f12
        local.tee $l9
        i32.const 28
        i32.mul
        local.tee $p3
        local.get $l9
        i32.const 153391689
        i32.gt_u
        select
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $p1
        i32.store offset=28
        local.get $l8
        i32.const 5
        i32.store offset=108
        local.get $p1
        i32.const 0
        local.get $p3
        call $f23
        local.set $l28
        local.get $p5
        i32.const 2
        i32.shr_u
        local.set $l12
        block $B33
          local.get $p2
          i32.eqz
          br_if $B33
          block $B34
            local.get $p4
            i32.eqz
            br_if $B34
            local.get $l12
            i32.const 2
            i32.shl
            local.set $l30
            local.get $l26
            local.set $l27
            local.get $l10
            local.set $p1
            local.get $p4
            local.set $l24
            local.get $p2
            local.set $l19
            loop $L35
              local.get $l28
              local.get $l27
              i32.load
              i32.const 28
              i32.mul
              i32.add
              local.tee $p3
              local.get $p1
              f32.load
              local.get $p3
              f32.load
              f32.add
              f32.store
              local.get $p3
              local.get $p1
              i32.const 4
              i32.add
              f32.load
              local.get $p3
              f32.load offset=4
              f32.add
              f32.store offset=4
              local.get $p3
              local.get $p1
              i32.const 8
              i32.add
              f32.load
              local.get $p3
              f32.load offset=8
              f32.add
              f32.store offset=8
              local.get $p3
              local.get $l24
              f32.load
              local.get $p3
              f32.load offset=12
              f32.add
              f32.store offset=12
              local.get $p3
              local.get $l24
              i32.const 4
              i32.add
              f32.load
              local.get $p3
              f32.load offset=16
              f32.add
              f32.store offset=16
              local.get $p3
              local.get $l24
              i32.const 8
              i32.add
              f32.load
              local.get $p3
              f32.load offset=20
              f32.add
              f32.store offset=20
              local.get $p3
              local.get $p3
              f32.load offset=24
              f32.const 0x1p+0 (;=1;)
              f32.add
              f32.store offset=24
              local.get $l27
              i32.const 4
              i32.add
              local.set $l27
              local.get $p1
              i32.const 12
              i32.add
              local.set $p1
              local.get $l24
              local.get $l30
              i32.add
              local.set $l24
              local.get $l19
              i32.const -1
              i32.add
              local.tee $l19
              br_if $L35
              br $B33
            end
          end
          local.get $l26
          local.set $l24
          local.get $l10
          local.set $p1
          local.get $p2
          local.set $l27
          loop $L36
            local.get $l28
            local.get $l24
            i32.load
            i32.const 28
            i32.mul
            i32.add
            local.tee $p3
            local.get $p1
            f32.load
            local.get $p3
            f32.load
            f32.add
            f32.store
            local.get $p3
            local.get $p1
            i32.const 4
            i32.add
            f32.load
            local.get $p3
            f32.load offset=4
            f32.add
            f32.store offset=4
            local.get $p3
            local.get $p1
            i32.const 8
            i32.add
            f32.load
            local.get $p3
            f32.load offset=8
            f32.add
            f32.store offset=8
            local.get $p3
            local.get $p3
            f32.load offset=12
            f32.const 0x0p+0 (;=0;)
            f32.add
            f32.store offset=12
            local.get $p3
            local.get $p3
            f32.load offset=16
            f32.const 0x0p+0 (;=0;)
            f32.add
            f32.store offset=16
            local.get $p3
            local.get $p3
            f32.load offset=20
            f32.const 0x0p+0 (;=0;)
            f32.add
            f32.store offset=20
            local.get $p3
            local.get $p3
            f32.load offset=24
            f32.const 0x1p+0 (;=1;)
            f32.add
            f32.store offset=24
            local.get $l24
            i32.const 4
            i32.add
            local.set $l24
            local.get $p1
            i32.const 12
            i32.add
            local.set $p1
            local.get $l27
            i32.const -1
            i32.add
            local.tee $l27
            br_if $L36
          end
        end
        block $B37
          local.get $l9
          i32.eqz
          br_if $B37
          local.get $l28
          local.set $p3
          local.get $l9
          local.set $p1
          loop $L38
            local.get $p3
            local.get $p3
            f32.load
            f32.const 0x0p+0 (;=0;)
            f32.const 0x1p+0 (;=1;)
            local.get $p3
            i32.const 24
            i32.add
            f32.load
            local.tee $l18
            f32.div
            local.get $l18
            f32.const 0x0p+0 (;=0;)
            f32.eq
            select
            local.tee $l18
            f32.mul
            f32.store
            local.get $p3
            i32.const 4
            i32.add
            local.tee $l24
            local.get $l18
            local.get $l24
            f32.load
            f32.mul
            f32.store
            local.get $p3
            i32.const 8
            i32.add
            local.tee $l24
            local.get $l18
            local.get $l24
            f32.load
            f32.mul
            f32.store
            local.get $p3
            i32.const 12
            i32.add
            local.tee $l24
            local.get $l18
            local.get $l24
            f32.load
            f32.mul
            f32.store
            local.get $p3
            i32.const 16
            i32.add
            local.tee $l24
            local.get $l18
            local.get $l24
            f32.load
            f32.mul
            f32.store
            local.get $p3
            i32.const 20
            i32.add
            local.tee $l24
            local.get $l18
            local.get $l24
            f32.load
            f32.mul
            f32.store
            local.get $p3
            i32.const 28
            i32.add
            local.set $p3
            local.get $p1
            i32.const -1
            i32.add
            local.tee $p1
            br_if $L38
          end
        end
        i32.const 0
        local.set $l27
        local.get $l8
        i32.const -1
        local.get $l9
        i32.const 2
        i32.shl
        local.tee $l20
        local.get $l9
        i32.const 1073741823
        i32.gt_u
        select
        local.tee $p3
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $p1
        i32.store offset=32
        local.get $l8
        i32.const 6
        i32.store offset=108
        local.get $l8
        local.get $p3
        i32.const 0
        i32.load offset=37592
        call_indirect $T0 (type $t0)
        local.tee $l30
        i32.store offset=36
        local.get $p1
        i32.const 255
        local.get $l20
        call $f23
        local.set $l29
        block $B39
          local.get $p2
          i32.eqz
          br_if $B39
          local.get $p6
          f32.const 0x1p+0 (;=1;)
          f32.const 0x1p+0 (;=1;)
          local.get $l25
          f32.div
          local.get $l14
          i32.const 1
          i32.eq
          select
          f32.mul
          local.tee $l18
          local.get $l18
          f32.mul
          local.set $l25
          local.get $l12
          i32.const 2
          i32.shl
          local.set $l12
          local.get $p4
          local.set $p1
          loop $L40
            local.get $l25
            local.get $p1
            i32.const 37012
            local.get $p4
            select
            local.tee $l24
            f32.load offset=8
            local.get $l28
            local.get $l26
            i32.load
            local.tee $l19
            i32.const 28
            i32.mul
            i32.add
            local.tee $p3
            f32.load offset=20
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            local.get $l24
            f32.load
            local.get $p3
            f32.load offset=12
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            local.get $l24
            f32.load offset=4
            local.get $p3
            f32.load offset=16
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            f32.add
            f32.add
            f32.mul
            local.get $l10
            i32.const 8
            i32.add
            f32.load
            local.get $p3
            f32.load offset=8
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            local.get $l10
            f32.load
            local.get $p3
            f32.load
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            local.get $l10
            i32.const 4
            i32.add
            f32.load
            local.get $p3
            f32.load offset=4
            f32.sub
            local.tee $l18
            local.get $l18
            f32.mul
            f32.add
            f32.add
            f32.add
            local.set $l18
            block $B41
              block $B42
                local.get $l29
                local.get $l19
                i32.const 2
                i32.shl
                local.tee $p3
                i32.add
                local.tee $l24
                i32.load
                i32.const -1
                i32.eq
                br_if $B42
                local.get $l30
                local.get $p3
                i32.add
                f32.load
                local.get $l18
                f32.gt
                i32.eqz
                br_if $B41
              end
              local.get $l24
              local.get $l27
              i32.store
              local.get $l30
              local.get $p3
              i32.add
              local.get $l18
              f32.store
            end
            local.get $l26
            i32.const 4
            i32.add
            local.set $l26
            local.get $l10
            i32.const 12
            i32.add
            local.set $l10
            local.get $p1
            local.get $l12
            i32.add
            local.set $p1
            local.get $p2
            local.get $l27
            i32.const 1
            i32.add
            local.tee $l27
            i32.ne
            br_if $L40
          end
        end
        local.get $p0
        local.get $l29
        local.get $l20
        call $f22
        drop
        i32.const 7
        local.set $p3
      end
      local.get $p3
      i32.const 2
      i32.shl
      local.set $p3
      local.get $l8
      i32.const 12
      i32.add
      i32.const -4
      i32.add
      local.set $p1
      loop $L43
        local.get $p1
        local.get $p3
        i32.add
        i32.load
        i32.const 0
        i32.load offset=37596
        call_indirect $T0 (type $t1)
        local.get $p3
        i32.const -4
        i32.add
        local.tee $p3
        br_if $L43
      end
    end
    local.get $l8
    i32.const 112
    i32.add
    global.set $g0
    local.get $l9)
  (func $meshopt_simplifyScale (type $t14) (param $p0 i32) (param $p1 i32) (param $p2 i32) (result f32)
    (local $l3 i32) (local $l4 i32) (local $l5 f32) (local $l6 f32) (local $l7 f32) (local $l8 i32) (local $l9 i32)
    i32.const 0
    local.set $l3
    global.get $g0
    i32.const 32
    i32.sub
    local.tee $l4
    i32.const 0
    i32.load offset=36872
    i32.store offset=24
    local.get $l4
    i32.const 0
    i64.load offset=36864 align=4
    i64.store offset=16
    local.get $l4
    i32.const 0
    i32.load offset=36884
    i32.store offset=8
    local.get $l4
    i32.const 0
    i64.load offset=36876 align=4
    i64.store
    block $B0
      block $B1
        local.get $p1
        br_if $B1
        f32.const -inf (;=-inf;)
        local.set $l5
        f32.const -inf (;=-inf;)
        local.set $l6
        f32.const -inf (;=-inf;)
        local.set $l7
        br $B0
      end
      local.get $p2
      i32.const 2
      i32.shr_u
      i32.const 2
      i32.shl
      local.set $l8
      loop $L2
        i32.const 0
        local.set $p2
        loop $L3
          local.get $l4
          i32.const 16
          i32.add
          local.get $p2
          i32.add
          local.tee $l9
          local.get $p0
          local.get $p2
          i32.add
          f32.load
          local.tee $l5
          local.get $l9
          f32.load
          local.tee $l6
          local.get $l6
          local.get $l5
          f32.gt
          select
          f32.store
          local.get $l4
          local.get $p2
          i32.add
          local.tee $l9
          local.get $l5
          local.get $l9
          f32.load
          local.tee $l6
          local.get $l6
          local.get $l5
          f32.lt
          select
          f32.store
          local.get $p2
          i32.const 4
          i32.add
          local.tee $p2
          i32.const 12
          i32.ne
          br_if $L3
        end
        local.get $p0
        local.get $l8
        i32.add
        local.set $p0
        local.get $l3
        i32.const 1
        i32.add
        local.tee $l3
        local.get $p1
        i32.ne
        br_if $L2
      end
      local.get $l4
      f32.load offset=8
      local.get $l4
      f32.load offset=24
      f32.sub
      local.set $l7
      local.get $l4
      f32.load offset=4
      local.get $l4
      f32.load offset=20
      f32.sub
      local.set $l6
      local.get $l4
      f32.load
      local.get $l4
      f32.load offset=16
      f32.sub
      local.set $l5
    end
    f32.const 0x0p+0 (;=0;)
    local.get $l5
    local.get $l5
    f32.const 0x0p+0 (;=0;)
    f32.lt
    select
    local.tee $l5
    local.get $l6
    local.get $l6
    local.get $l5
    f32.lt
    select
    local.tee $l5
    local.get $l7
    local.get $l7
    local.get $l5
    f32.lt
    select)
  (func $meshopt_remesh (type $t15) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 i32) (result i32)
    (local $l9 i32) (local $l10 i32) (local $l11 f32) (local $l12 f32) (local $l13 f32) (local $l14 i32) (local $l15 i32) (local $l16 i32) (local $l17 i32) (local $l18 f32) (local $l19 f32) (local $l20 i32) (local $l21 i32) (local $l22 i32) (local $l23 i32) (local $l24 i32) (local $l25 i32) (local $l26 i32) (local $l27 i32) (local $l28 i32) (local $l29 i32) (local $l30 i32) (local $l31 i32) (local $l32 i32) (local $l33 i32) (local $l34 i32) (local $l35 i32) (local $l36 i32) (local $l37 i32) (local $l38 i32) (local $l39 f32) (local $l40 f32) (local $l41 f32) (local $l42 f32) (local $l43 f32) (local $l44 f32) (local $l45 f32) (local $l46 f32) (local $l47 f32) (local $l48 f32) (local $l49 f32) (local $l50 f32) (local $l51 f32) (local $l52 f32) (local $l53 f32)
    global.get $g0
    i32.const 384
    i32.sub
    local.tee $l9
    global.set $g0
    i32.const 0
    local.set $l10
    local.get $l9
    i32.const 12
    i32.add
    i32.const 0
    i32.const 100
    call $f23
    drop
    local.get $l9
    i32.const 0
    i32.store offset=8
    local.get $l9
    i64.const 0
    i64.store
    local.get $l9
    i32.const 0
    i32.load offset=37576
    i32.store offset=136
    local.get $l9
    i32.const 0
    i64.load offset=37568 align=4
    i64.store offset=128
    local.get $l9
    i32.const 0
    i32.load offset=37588
    i32.store offset=120
    local.get $l9
    i32.const 0
    i64.load offset=37580 align=4
    i64.store offset=112
    block $B0
      block $B1
        local.get $p5
        br_if $B1
        f32.const -inf (;=-inf;)
        local.set $l11
        f32.const -inf (;=-inf;)
        local.set $l12
        f32.const -inf (;=-inf;)
        local.set $l13
        br $B0
      end
      local.get $p6
      i32.const 2
      i32.shr_u
      i32.const 2
      i32.shl
      local.set $l14
      local.get $p4
      local.set $l15
      loop $L2
        i32.const 0
        local.set $l16
        loop $L3
          local.get $l9
          i32.const 128
          i32.add
          local.get $l16
          i32.add
          local.tee $l17
          local.get $l15
          local.get $l16
          i32.add
          f32.load
          local.tee $l11
          local.get $l17
          f32.load
          local.tee $l12
          local.get $l12
          local.get $l11
          f32.gt
          select
          f32.store
          local.get $l9
          i32.const 112
          i32.add
          local.get $l16
          i32.add
          local.tee $l17
          local.get $l11
          local.get $l17
          f32.load
          local.tee $l12
          local.get $l12
          local.get $l11
          f32.lt
          select
          f32.store
          local.get $l16
          i32.const 4
          i32.add
          local.tee $l16
          i32.const 12
          i32.ne
          br_if $L3
        end
        local.get $l15
        local.get $l14
        i32.add
        local.set $l15
        local.get $l10
        i32.const 1
        i32.add
        local.tee $l10
        local.get $p5
        i32.ne
        br_if $L2
      end
      local.get $l9
      f32.load offset=120
      local.get $l9
      f32.load offset=136
      f32.sub
      local.set $l13
      local.get $l9
      f32.load offset=116
      local.get $l9
      f32.load offset=132
      f32.sub
      local.set $l12
      local.get $l9
      f32.load offset=112
      local.get $l9
      f32.load offset=128
      f32.sub
      local.set $l11
    end
    f32.const 0x0p+0 (;=0;)
    local.set $l18
    block $B4
      local.get $p7
      f32.convert_i32_s
      local.tee $l19
      f32.const 0x1.47ae14p-7 (;=0.01;)
      f32.add
      local.get $l19
      f32.div
      f32.const 0x0p+0 (;=0;)
      local.get $l11
      local.get $l11
      f32.const 0x0p+0 (;=0;)
      f32.lt
      select
      local.tee $l11
      local.get $l12
      local.get $l12
      local.get $l11
      f32.lt
      select
      local.tee $l11
      local.get $l13
      local.get $l13
      local.get $l11
      f32.lt
      select
      f32.mul
      local.tee $l12
      f32.const 0x0p+0 (;=0;)
      f32.eq
      br_if $B4
      local.get $p7
      i32.const -2
      i32.add
      f32.convert_i32_s
      local.get $l12
      f32.div
      local.set $l18
    end
    local.get $l9
    i32.const 20
    i32.add
    local.set $l20
    local.get $l9
    i32.const 12
    i32.add
    i32.const 12
    i32.add
    local.set $l21
    i32.const 0
    local.set $l16
    loop $L5
      local.get $l9
      local.get $l16
      i32.add
      local.get $l9
      i32.const 128
      i32.add
      local.get $l16
      i32.add
      f32.load
      local.tee $l11
      local.get $l12
      local.get $l9
      i32.const 112
      i32.add
      local.get $l16
      i32.add
      f32.load
      local.get $l11
      f32.sub
      f32.sub
      f32.const -0x1p-1 (;=-0.5;)
      f32.mul
      f32.add
      f32.store
      local.get $l16
      i32.const 4
      i32.add
      local.tee $l16
      i32.const 12
      i32.ne
      br_if $L5
    end
    i32.const 0
    local.set $l22
    local.get $l9
    local.get $p7
    local.get $p7
    i32.mul
    local.tee $l23
    local.get $p7
    i32.mul
    local.tee $l16
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $l17
    i32.store offset=12
    local.get $l9
    i32.const 1
    i32.store offset=108
    local.get $l17
    i32.const 0
    local.get $l16
    call $f23
    local.tee $l24
    i32.const 0
    i32.const 0
    local.get $p2
    local.get $p3
    local.get $p4
    local.get $p6
    local.get $p7
    local.get $l18
    local.get $l9
    local.get $p8
    call $f17
    i32.const 2
    local.set $l25
    local.get $l9
    i32.const -1
    local.get $l23
    i32.const 2
    i32.shl
    local.get $l23
    i32.const 1073741823
    i32.gt_u
    select
    local.tee $l26
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $l27
    i32.store offset=16
    local.get $l9
    i32.const 2
    i32.store offset=108
    local.get $l9
    i32.const 128
    i32.add
    i32.const 0
    local.get $p7
    call $f23
    drop
    i32.const 0
    local.set $l28
    block $B6
      local.get $l23
      i32.eqz
      br_if $B6
      block $B7
        local.get $p7
        i32.const 0
        i32.gt_s
        br_if $B7
        local.get $l24
        local.set $l16
        local.get $l27
        local.set $l17
        local.get $l23
        local.set $l15
        loop $L8
          i32.const 0
          local.set $l28
          local.get $l17
          i32.const 0
          i32.const -1
          local.get $l16
          local.get $l9
          i32.const 128
          i32.add
          local.get $p7
          call $f26
          select
          i32.store
          local.get $l16
          local.get $p7
          i32.add
          local.set $l16
          local.get $l17
          i32.const 4
          i32.add
          local.set $l17
          local.get $l15
          i32.const -1
          i32.add
          local.tee $l15
          br_if $L8
          br $B6
        end
      end
      local.get $l24
      local.set $l14
      i32.const 0
      local.set $l28
      i32.const 0
      local.set $p5
      loop $L9
        block $B10
          block $B11
            local.get $l24
            local.get $p5
            local.get $p7
            i32.mul
            i32.add
            local.get $l9
            i32.const 128
            i32.add
            local.get $p7
            call $f26
            i32.eqz
            br_if $B11
            i32.const 0
            local.set $l17
            local.get $l14
            local.set $l16
            local.get $p7
            local.set $l15
            loop $L12
              local.get $l16
              local.get $l17
              local.get $l16
              i32.load8_u
              local.tee $l10
              i32.add
              local.tee $l17
              i32.const 0
              local.get $l10
              select
              i32.store8
              local.get $l16
              i32.const 1
              i32.add
              local.set $l16
              local.get $l15
              i32.const -1
              i32.add
              local.tee $l15
              br_if $L12
            end
            local.get $l27
            local.get $p5
            i32.const 2
            i32.shl
            i32.add
            local.get $l28
            i32.store
            local.get $l17
            local.get $l28
            i32.add
            local.set $l28
            br $B10
          end
          local.get $l27
          local.get $p5
          i32.const 2
          i32.shl
          i32.add
          i32.const -1
          i32.store
        end
        local.get $l14
        local.get $p7
        i32.add
        local.set $l14
        local.get $p5
        i32.const 1
        i32.add
        local.tee $p5
        local.get $l23
        i32.ne
        br_if $L9
      end
    end
    block $B13
      local.get $p0
      i32.eqz
      br_if $B13
      local.get $l9
      i32.const -1
      local.get $l28
      i32.const 6
      i32.shl
      local.tee $l16
      local.get $l28
      i32.const 67108863
      i32.gt_u
      select
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l22
      i32.store offset=20
      i32.const 3
      local.set $l25
      local.get $l9
      i32.const 3
      i32.store offset=108
      local.get $l22
      i32.const 0
      local.get $l16
      call $f23
      drop
      local.get $l21
      local.set $l20
    end
    block $B14
      local.get $p8
      i32.const 1
      i32.and
      br_if $B14
      local.get $l20
      local.get $l26
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l29
      i32.store
      local.get $l9
      local.get $l25
      i32.const 1
      i32.add
      local.tee $l16
      i32.store offset=108
      local.get $l9
      i32.const 12
      i32.add
      local.get $l16
      i32.const 2
      i32.shl
      i32.add
      local.get $l23
      i32.const 0
      i32.load offset=37592
      call_indirect $T0 (type $t0)
      local.tee $l16
      i32.store
      local.get $l9
      local.get $l25
      i32.const 2
      i32.add
      local.tee $l25
      i32.store offset=108
      local.get $l16
      i32.const 0
      local.get $l23
      call $f23
      local.set $l30
      local.get $p7
      i32.const -1
      i32.add
      local.set $l21
      block $B15
        local.get $p7
        i32.const 3
        i32.lt_s
        br_if $B15
        local.get $p7
        i32.const -2
        i32.add
        local.set $l20
        local.get $p7
        local.get $p7
        i32.const 1
        i32.add
        i32.mul
        local.get $l24
        i32.add
        i32.const 1
        i32.add
        local.set $l26
        i32.const 1
        local.set $l31
        loop $L16
          local.get $l31
          local.get $p7
          i32.mul
          local.set $l14
          local.get $l26
          local.set $p5
          i32.const 1
          local.set $l10
          loop $L17
            block $B18
              local.get $l27
              local.get $l10
              local.get $l14
              i32.add
              i32.const 2
              i32.shl
              i32.add
              i32.load
              i32.const -1
              i32.eq
              br_if $B18
              local.get $p5
              local.set $l16
              local.get $l20
              local.set $l17
              loop $L19
                local.get $l16
                local.get $l16
                i32.load8_u
                local.tee $l15
                i32.const -1
                local.get $l15
                select
                i32.store8
                local.get $l16
                i32.const 1
                i32.add
                local.set $l16
                local.get $l17
                i32.const -1
                i32.add
                local.tee $l17
                br_if $L19
              end
            end
            local.get $p5
            local.get $p7
            i32.add
            local.set $p5
            local.get $l10
            i32.const 1
            i32.add
            local.tee $l10
            local.get $l21
            i32.ne
            br_if $L17
          end
          local.get $l26
          local.get $l23
          i32.add
          local.set $l26
          local.get $l31
          i32.const 1
          i32.add
          local.tee $l31
          local.get $l21
          i32.ne
          br_if $L16
        end
      end
      local.get $p7
      i32.const 1
      i32.lt_s
      br_if $B14
      i32.const 0
      local.set $l26
      i32.const 0
      local.set $l10
      i32.const 0
      local.set $p5
      loop $L20
        local.get $l30
        local.get $l10
        i32.add
        local.set $l15
        i32.const 0
        local.set $l16
        loop $L21
          block $B22
            local.get $l15
            local.get $l16
            i32.add
            local.tee $l17
            i32.load8_u
            br_if $B22
            local.get $l17
            i32.const 1
            i32.store8
            local.get $l29
            local.get $l26
            i32.const 2
            i32.shl
            i32.add
            local.get $l10
            local.get $l16
            i32.add
            i32.store
            local.get $l26
            i32.const 1
            i32.add
            local.set $l26
          end
          local.get $p7
          local.get $l16
          i32.const 1
          i32.add
          local.tee $l16
          i32.ne
          br_if $L21
        end
        local.get $l10
        local.get $p7
        i32.add
        local.set $l10
        local.get $p5
        i32.const 1
        i32.add
        local.tee $p5
        local.get $p7
        i32.ne
        br_if $L20
      end
      local.get $l26
      i32.eqz
      br_if $B14
      local.get $l24
      i32.const 1
      i32.add
      local.set $l32
      local.get $l24
      i32.const -2
      i32.add
      local.set $l33
      local.get $p7
      i32.const -2
      i32.add
      local.set $l34
      local.get $p7
      i32.const 3
      i32.lt_s
      local.set $l35
      loop $L23
        local.get $l30
        local.get $l29
        local.get $l26
        i32.const -1
        i32.add
        local.tee $l26
        i32.const 2
        i32.shl
        i32.add
        i32.load
        local.tee $p5
        i32.add
        i32.const 0
        i32.store8
        local.get $p5
        local.get $p7
        i32.mul
        local.set $l14
        block $B24
          local.get $l35
          br_if $B24
          local.get $l27
          local.get $p5
          i32.const 2
          i32.shl
          i32.add
          i32.load
          i32.const -1
          i32.eq
          br_if $B24
          local.get $l24
          local.get $l14
          i32.add
          local.set $l17
          local.get $l34
          local.set $l10
          loop $L25
            block $B26
              local.get $l17
              i32.const 1
              i32.add
              local.tee $l16
              i32.load8_u
              local.tee $l15
              i32.const 255
              i32.ne
              br_if $B26
              local.get $l17
              i32.load8_u
              br_if $B26
              i32.const 0
              local.set $l15
            end
            local.get $l16
            local.get $l15
            i32.store8
            local.get $l16
            local.set $l17
            local.get $l10
            i32.const -1
            i32.add
            local.tee $l10
            br_if $L25
          end
          local.get $l33
          local.get $p7
          local.get $p5
          i32.const 1
          i32.add
          i32.mul
          i32.add
          local.set $l16
          local.get $l21
          local.set $l15
          loop $L27
            block $B28
              local.get $l16
              i32.load8_u
              local.tee $l17
              i32.const 255
              i32.ne
              br_if $B28
              local.get $l16
              i32.const 1
              i32.add
              i32.load8_u
              br_if $B28
              i32.const 0
              local.set $l17
            end
            local.get $l16
            local.get $l17
            i32.store8
            local.get $l16
            i32.const -1
            i32.add
            local.set $l16
            local.get $l15
            i32.const -1
            i32.add
            local.tee $l15
            i32.const 1
            i32.gt_s
            br_if $L27
          end
        end
        local.get $l32
        local.get $l14
        i32.add
        local.set $l36
        local.get $p5
        local.get $p5
        local.get $p7
        i32.div_u
        local.tee $l37
        local.get $p7
        i32.mul
        i32.sub
        local.set $l31
        i32.const 0
        local.set $l20
        loop $L29
          block $B30
            local.get $l20
            i32.const 1
            i32.eq
            i32.const -1
            local.get $l20
            select
            local.get $l31
            i32.add
            local.tee $l16
            i32.const 1
            i32.lt_s
            br_if $B30
            local.get $l16
            local.get $l21
            i32.ge_s
            br_if $B30
            i32.const -1
            local.get $l20
            i32.const 3
            i32.eq
            local.get $l20
            i32.const 2
            i32.eq
            select
            local.get $l37
            i32.add
            local.tee $l17
            i32.const 1
            i32.lt_s
            br_if $B30
            local.get $l17
            local.get $l21
            i32.ge_s
            br_if $B30
            local.get $l27
            local.get $l17
            local.get $p7
            i32.mul
            local.get $l16
            i32.add
            local.tee $l38
            i32.const 2
            i32.shl
            i32.add
            i32.load
            i32.const -1
            i32.eq
            br_if $B30
            local.get $l35
            br_if $B30
            local.get $l32
            local.get $l38
            local.get $p7
            i32.mul
            i32.add
            local.set $l16
            i32.const 0
            local.set $l10
            local.get $l36
            local.set $l15
            local.get $l34
            local.set $p5
            loop $L31
              local.get $l16
              local.get $l16
              i32.load8_u
              local.tee $l17
              i32.const 0
              local.get $l17
              local.get $l17
              i32.const 255
              i32.eq
              select
              local.get $l15
              i32.load8_u
              select
              local.tee $l14
              i32.store8
              local.get $l15
              i32.const 1
              i32.add
              local.set $l15
              local.get $l16
              i32.const 1
              i32.add
              local.set $l16
              local.get $l14
              local.get $l17
              i32.xor
              local.get $l10
              i32.or
              local.set $l10
              local.get $p5
              i32.const -1
              i32.add
              local.tee $p5
              br_if $L31
            end
            local.get $l10
            i32.const 255
            i32.and
            i32.eqz
            br_if $B30
            local.get $l30
            local.get $l38
            i32.add
            local.tee $l16
            i32.load8_u
            br_if $B30
            local.get $l16
            i32.const 1
            i32.store8
            local.get $l29
            local.get $l26
            i32.const 2
            i32.shl
            i32.add
            local.get $l38
            i32.store
            local.get $l26
            i32.const 1
            i32.add
            local.set $l26
          end
          local.get $l20
          i32.const 1
          i32.add
          local.tee $l20
          i32.const 4
          i32.ne
          br_if $L29
        end
        local.get $l26
        br_if $L23
      end
    end
    block $B32
      local.get $l22
      i32.eqz
      br_if $B32
      local.get $l24
      local.get $l27
      local.get $l22
      local.get $p2
      local.get $p3
      local.get $p4
      local.get $p6
      local.get $p7
      local.get $l18
      local.get $l9
      local.get $p8
      call $f17
      local.get $l28
      i32.eqz
      br_if $B32
      f32.const 0x1p+0 (;=1;)
      local.get $l18
      f32.div
      local.set $l11
      local.get $p8
      i32.const 1073741824
      i32.and
      local.set $l14
      block $B33
        local.get $p8
        i32.const 2
        i32.and
        i32.eqz
        br_if $B33
        f32.const 0x1.8p+1 (;=3;)
        local.get $l18
        local.get $l18
        f32.mul
        f32.div
        local.set $l39
        local.get $l11
        f32.const 0x1p-1 (;=0.5;)
        f32.mul
        local.set $l40
        local.get $l22
        local.set $l16
        loop $L34
          local.get $l16
          i32.const 16
          i32.add
          local.tee $l15
          f32.load
          local.tee $l41
          local.get $l16
          i32.const 20
          i32.add
          f32.load
          local.tee $l12
          f32.div
          local.set $l13
          local.get $l16
          i32.const 12
          i32.add
          local.tee $l10
          f32.load
          local.tee $l42
          local.get $l12
          f32.div
          local.set $l19
          local.get $l16
          i32.const 8
          i32.add
          local.tee $p5
          f32.load
          local.tee $l43
          local.get $l12
          f32.div
          local.set $l44
          local.get $l11
          local.get $l16
          i32.load
          local.tee $l17
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          local.set $l45
          local.get $l11
          local.get $l17
          i32.const 10
          i32.shr_u
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          local.set $l46
          local.get $l11
          local.get $l17
          i32.const 20
          i32.shr_u
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          local.set $l47
          block $B35
            local.get $l12
            f32.const 0x1.eb851ep-6 (;=0.03;)
            f32.mul
            local.tee $l48
            local.get $l16
            i32.const 24
            i32.add
            f32.load
            f32.add
            local.tee $l49
            f32.abs
            local.get $l12
            f32.const 0x1.0c6f7ap-20 (;=1e-06;)
            f32.mul
            local.tee $l12
            f32.gt
            i32.eqz
            br_if $B35
            local.get $l48
            local.get $l16
            i32.const 28
            i32.add
            f32.load
            f32.add
            local.get $l16
            i32.const 36
            i32.add
            f32.load
            local.tee $l50
            local.get $l50
            local.get $l49
            f32.div
            local.tee $l50
            f32.mul
            f32.sub
            local.tee $l51
            f32.abs
            local.get $l12
            f32.gt
            i32.eqz
            br_if $B35
            local.get $l48
            local.get $l16
            i32.const 32
            i32.add
            f32.load
            f32.add
            local.get $l16
            i32.const 40
            i32.add
            f32.load
            local.tee $l48
            local.get $l48
            local.get $l49
            f32.div
            local.tee $l52
            f32.mul
            f32.sub
            local.get $l16
            i32.const 44
            i32.add
            f32.load
            local.get $l48
            local.get $l50
            f32.mul
            f32.sub
            local.tee $l48
            local.get $l48
            local.get $l51
            f32.div
            local.tee $l48
            f32.mul
            f32.sub
            local.tee $l53
            f32.abs
            local.get $l12
            f32.gt
            i32.eqz
            br_if $B35
            local.get $l41
            f32.const 0x1.eb851ep-6 (;=0.03;)
            f32.mul
            local.get $l16
            i32.const 56
            i32.add
            f32.load
            f32.sub
            local.get $l52
            local.get $l43
            f32.const 0x1.eb851ep-6 (;=0.03;)
            f32.mul
            local.get $l16
            i32.const 48
            i32.add
            f32.load
            f32.sub
            local.tee $l41
            f32.mul
            f32.sub
            local.get $l48
            local.get $l42
            f32.const 0x1.eb851ep-6 (;=0.03;)
            f32.mul
            local.get $l16
            i32.const 52
            i32.add
            f32.load
            f32.sub
            local.get $l50
            local.get $l41
            f32.mul
            f32.sub
            local.tee $l42
            f32.mul
            f32.sub
            local.get $l53
            f32.div
            local.tee $l12
            local.get $l13
            f32.sub
            local.tee $l43
            local.get $l43
            f32.mul
            local.get $l41
            local.get $l49
            f32.div
            local.get $l50
            local.get $l42
            local.get $l51
            f32.div
            local.get $l48
            local.get $l12
            f32.mul
            f32.sub
            local.tee $l49
            f32.mul
            f32.sub
            local.get $l52
            local.get $l12
            f32.mul
            f32.sub
            local.tee $l48
            local.get $l44
            f32.sub
            local.tee $l50
            local.get $l50
            f32.mul
            local.get $l49
            local.get $l19
            f32.sub
            local.tee $l50
            local.get $l50
            f32.mul
            f32.add
            f32.add
            local.get $l39
            f32.lt
            i32.eqz
            br_if $B35
            local.get $l12
            local.get $l45
            local.get $l12
            local.get $l45
            f32.gt
            select
            local.tee $l12
            local.get $l11
            local.get $l45
            f32.add
            local.tee $l13
            local.get $l12
            local.get $l13
            f32.lt
            select
            local.set $l13
            local.get $l49
            local.get $l46
            local.get $l49
            local.get $l46
            f32.gt
            select
            local.tee $l12
            local.get $l11
            local.get $l46
            f32.add
            local.tee $l19
            local.get $l12
            local.get $l19
            f32.lt
            select
            local.set $l19
            local.get $l48
            local.get $l47
            local.get $l48
            local.get $l47
            f32.gt
            select
            local.tee $l12
            local.get $l11
            local.get $l47
            f32.add
            local.tee $l44
            local.get $l12
            local.get $l44
            f32.lt
            select
            local.set $l44
          end
          block $B36
            local.get $l14
            i32.eqz
            br_if $B36
            local.get $l40
            local.get $l45
            f32.add
            local.set $l13
            local.get $l40
            local.get $l46
            f32.add
            local.set $l19
            local.get $l40
            local.get $l47
            f32.add
            local.set $l44
          end
          local.get $l15
          local.get $l13
          f32.store
          local.get $l10
          local.get $l19
          f32.store
          local.get $p5
          local.get $l44
          f32.store
          local.get $l16
          i32.const 64
          i32.add
          local.set $l16
          local.get $l28
          i32.const -1
          i32.add
          local.tee $l28
          br_if $L34
          br $B32
        end
      end
      block $B37
        local.get $l14
        i32.eqz
        br_if $B37
        local.get $l11
        f32.const 0x1p-1 (;=0.5;)
        f32.mul
        local.set $l12
        local.get $l22
        local.set $l16
        loop $L38
          local.get $l16
          i32.const 16
          i32.add
          local.get $l12
          local.get $l11
          local.get $l16
          i32.load
          local.tee $l17
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          f32.add
          f32.store
          local.get $l16
          i32.const 12
          i32.add
          local.get $l12
          local.get $l11
          local.get $l17
          i32.const 10
          i32.shr_u
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          f32.add
          f32.store
          local.get $l16
          i32.const 8
          i32.add
          local.get $l12
          local.get $l11
          local.get $l17
          i32.const 20
          i32.shr_u
          i32.const 1023
          i32.and
          f32.convert_i32_u
          f32.mul
          f32.add
          f32.store
          local.get $l16
          i32.const 64
          i32.add
          local.set $l16
          local.get $l28
          i32.const -1
          i32.add
          local.tee $l28
          br_if $L38
          br $B32
        end
      end
      local.get $l22
      i32.const 20
      i32.add
      local.set $l16
      loop $L39
        local.get $l16
        i32.const -4
        i32.add
        local.tee $l17
        local.get $l17
        f32.load
        local.get $l16
        f32.load
        local.tee $l11
        f32.div
        f32.store
        local.get $l16
        i32.const -8
        i32.add
        local.tee $l17
        local.get $l17
        f32.load
        local.get $l11
        f32.div
        f32.store
        local.get $l16
        i32.const -12
        i32.add
        local.tee $l17
        local.get $l17
        f32.load
        local.get $l11
        f32.div
        f32.store
        local.get $l16
        i32.const 64
        i32.add
        local.set $l16
        local.get $l28
        i32.const -1
        i32.add
        local.tee $l28
        br_if $L39
      end
    end
    i32.const 0
    local.set $l15
    block $B40
      block $B41
        local.get $p7
        i32.const 2
        i32.lt_s
        br_if $B41
        local.get $p7
        i32.const -1
        i32.add
        local.set $l28
        local.get $l27
        local.get $p7
        i32.const 2
        i32.shl
        i32.add
        local.set $l32
        block $B42
          local.get $p0
          br_if $B42
          local.get $l24
          i32.const 1
          i32.add
          local.tee $l38
          local.get $p7
          local.get $p7
          i32.const 1
          i32.add
          local.tee $l16
          i32.mul
          i32.add
          local.set $l35
          local.get $l24
          local.get $l16
          i32.add
          local.set $p0
          local.get $l38
          local.get $l23
          i32.add
          local.set $l30
          i32.const 0
          local.set $l15
          i32.const 0
          local.set $l36
          loop $L43
            local.get $l36
            local.get $p7
            i32.mul
            local.set $l37
            local.get $l38
            local.set $p1
            local.get $p0
            local.set $l26
            local.get $l30
            local.set $l31
            local.get $l35
            local.set $l29
            i32.const 0
            local.set $l21
            loop $L44
              block $B45
                local.get $l27
                local.get $l21
                local.get $l37
                i32.add
                local.tee $l10
                i32.const 2
                i32.shl
                local.tee $l16
                i32.add
                local.tee $l17
                i32.const 4
                i32.add
                i32.load
                local.get $l17
                i32.load
                i32.and
                local.get $l32
                local.get $l16
                i32.add
                local.tee $l16
                i32.load
                i32.and
                local.get $l16
                i32.const 4
                i32.add
                i32.load
                i32.and
                i32.const -1
                i32.eq
                br_if $B45
                i32.const 4
                i32.const 0
                local.get $l24
                local.get $l10
                local.get $p7
                i32.mul
                i32.add
                local.tee $l16
                local.get $p7
                i32.add
                i32.load8_u
                select
                local.get $l16
                i32.load8_u
                i32.const 0
                i32.ne
                i32.or
                i32.const 16
                i32.const 0
                local.get $l16
                local.get $l23
                i32.add
                local.tee $l16
                i32.load8_u
                select
                i32.or
                i32.const 64
                i32.const 0
                local.get $l16
                local.get $p7
                i32.add
                i32.load8_u
                select
                i32.or
                local.set $l14
                local.get $p1
                local.set $l16
                local.get $l26
                local.set $l17
                local.get $l31
                local.set $l10
                local.get $l29
                local.set $p5
                local.get $l28
                local.set $l22
                loop $L46
                  block $B47
                    local.get $l14
                    i32.const 4
                    i32.const 0
                    local.get $l17
                    i32.load8_u
                    select
                    local.get $l16
                    i32.load8_u
                    i32.const 0
                    i32.ne
                    i32.or
                    i32.const 16
                    i32.const 0
                    local.get $l10
                    i32.load8_u
                    select
                    i32.or
                    i32.const 64
                    i32.const 0
                    local.get $p5
                    i32.load8_u
                    select
                    i32.or
                    local.tee $l14
                    i32.const 1
                    i32.shl
                    i32.or
                    local.tee $l20
                    i32.eqz
                    br_if $B47
                    local.get $l20
                    i32.const 255
                    i32.eq
                    br_if $B47
                    local.get $l15
                    local.get $l20
                    i32.load8_u offset=42992
                    i32.add
                    local.set $l15
                  end
                  local.get $l16
                  i32.const 1
                  i32.add
                  local.set $l16
                  local.get $l17
                  i32.const 1
                  i32.add
                  local.set $l17
                  local.get $l10
                  i32.const 1
                  i32.add
                  local.set $l10
                  local.get $p5
                  i32.const 1
                  i32.add
                  local.set $p5
                  local.get $l22
                  i32.const -1
                  i32.add
                  local.tee $l22
                  br_if $L46
                end
              end
              local.get $p1
              local.get $p7
              i32.add
              local.set $p1
              local.get $l26
              local.get $p7
              i32.add
              local.set $l26
              local.get $l31
              local.get $p7
              i32.add
              local.set $l31
              local.get $l29
              local.get $p7
              i32.add
              local.set $l29
              local.get $l21
              i32.const 1
              i32.add
              local.tee $l21
              local.get $l28
              i32.ne
              br_if $L44
            end
            local.get $l38
            local.get $l23
            i32.add
            local.set $l38
            local.get $p0
            local.get $l23
            i32.add
            local.set $p0
            local.get $l30
            local.get $l23
            i32.add
            local.set $l30
            local.get $l35
            local.get $l23
            i32.add
            local.set $l35
            local.get $l36
            i32.const 1
            i32.add
            local.tee $l36
            local.get $l28
            i32.ne
            br_if $L43
            br $B41
          end
        end
        f32.const 0x1p+0 (;=1;)
        local.get $l18
        f32.div
        local.tee $l11
        local.get $l11
        f32.const 0x1.47ae16p-7 (;=0.01;)
        f32.mul
        f32.mul
        local.set $l43
        local.get $p8
        i32.const 2
        i32.and
        local.set $l30
        local.get $l9
        f32.load offset=8
        local.set $l11
        local.get $l9
        f32.load offset=4
        local.set $l12
        local.get $l9
        f32.load
        local.set $l13
        i32.const 0
        local.set $l15
        i32.const 0
        local.set $l14
        loop $L48
          local.get $l14
          local.get $p7
          i32.mul
          local.set $l36
          i32.const 0
          local.set $p5
          loop $L49
            block $B50
              local.get $l27
              local.get $p5
              local.get $l36
              i32.add
              local.tee $l10
              i32.const 2
              i32.shl
              local.tee $l16
              i32.add
              local.tee $l17
              i32.const 4
              i32.add
              i32.load
              local.get $l17
              i32.load
              i32.and
              local.get $l32
              local.get $l16
              i32.add
              local.tee $l16
              i32.load
              i32.and
              local.get $l16
              i32.const 4
              i32.add
              i32.load
              i32.and
              i32.const -1
              i32.eq
              br_if $B50
              i32.const 0
              local.set $l26
              i32.const 4
              i32.const 0
              local.get $l24
              local.get $l10
              local.get $p7
              i32.mul
              i32.add
              local.tee $l25
              local.get $p7
              i32.add
              i32.load8_u
              select
              local.get $l25
              i32.load8_u
              i32.const 0
              i32.ne
              i32.or
              i32.const 16
              i32.const 0
              local.get $l25
              local.get $l23
              i32.add
              local.tee $l35
              i32.load8_u
              select
              i32.or
              i32.const 64
              i32.const 0
              local.get $l35
              local.get $p7
              i32.add
              i32.load8_u
              select
              i32.or
              local.set $l37
              loop $L51
                block $B52
                  local.get $l37
                  i32.const 4
                  i32.const 0
                  local.get $l25
                  local.get $l26
                  local.tee $l38
                  i32.const 1
                  i32.add
                  local.tee $l26
                  i32.add
                  local.tee $l16
                  local.get $p7
                  i32.add
                  i32.load8_u
                  select
                  local.get $l16
                  i32.load8_u
                  i32.const 0
                  i32.ne
                  i32.or
                  i32.const 16
                  i32.const 0
                  local.get $l35
                  local.get $l26
                  i32.add
                  local.tee $l16
                  i32.load8_u
                  select
                  i32.or
                  i32.const 64
                  i32.const 0
                  local.get $l16
                  local.get $p7
                  i32.add
                  i32.load8_u
                  select
                  i32.or
                  local.tee $l37
                  i32.const 1
                  i32.shl
                  i32.or
                  local.tee $l20
                  i32.eqz
                  br_if $B52
                  local.get $l20
                  i32.const 255
                  i32.eq
                  br_if $B52
                  i32.const 0
                  local.set $l10
                  block $B53
                    block $B54
                      block $B55
                        local.get $l20
                        i32.load8_u offset=42736
                        i32.const -1
                        i32.add
                        br_table $B54 $B55 $B53
                      end
                      local.get $l30
                      i32.eqz
                      br_if $B53
                      local.get $l20
                      i32.const 20
                      i32.mul
                      local.tee $l16
                      i32.load16_u offset=37616
                      i32.const 4
                      i32.shl
                      local.get $l16
                      i32.load16_u offset=37618
                      i32.const 15
                      i32.and
                      i32.or
                      local.set $l31
                      local.get $l24
                      local.get $l38
                      i32.add
                      local.set $l29
                      i32.const 12
                      local.set $l16
                      local.get $l9
                      i32.const 128
                      i32.add
                      local.set $l17
                      loop $L56
                        local.get $l17
                        local.get $l22
                        local.get $l27
                        local.get $l31
                        local.get $l16
                        i32.shr_u
                        local.tee $l10
                        i32.const 1
                        i32.shr_u
                        i32.const 1
                        i32.and
                        local.get $p5
                        i32.add
                        local.get $l10
                        i32.const 2
                        i32.shr_u
                        i32.const 1
                        i32.and
                        local.get $l14
                        i32.add
                        local.get $p7
                        i32.mul
                        i32.add
                        local.tee $l21
                        i32.const 2
                        i32.shl
                        i32.add
                        i32.load
                        i32.const 6
                        i32.shl
                        i32.add
                        local.get $l29
                        local.get $l10
                        i32.const 1
                        i32.and
                        i32.add
                        local.get $l21
                        local.get $p7
                        i32.mul
                        i32.add
                        i32.load8_u
                        i32.const 6
                        i32.shl
                        i32.add
                        i32.const -64
                        i32.add
                        i32.store
                        local.get $l17
                        i32.const 4
                        i32.add
                        local.set $l17
                        local.get $l16
                        i32.const -4
                        i32.add
                        local.tee $l16
                        i32.const -4
                        i32.ne
                        br_if $L56
                      end
                      local.get $l9
                      i32.load offset=128
                      local.tee $l16
                      f32.load offset=40
                      local.get $l9
                      i32.load offset=140
                      local.tee $l17
                      f32.load offset=40
                      f32.add
                      local.get $l9
                      i32.load offset=132
                      local.tee $l10
                      f32.load offset=40
                      local.get $l9
                      i32.load offset=136
                      local.tee $l21
                      f32.load offset=40
                      f32.add
                      f32.add
                      local.tee $l49
                      local.get $l10
                      f32.load offset=8
                      local.get $l21
                      f32.load offset=8
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l19
                      f32.mul
                      local.get $l16
                      f32.load offset=56
                      local.get $l17
                      f32.load offset=56
                      f32.add
                      local.get $l10
                      f32.load offset=56
                      local.get $l21
                      f32.load offset=56
                      f32.add
                      f32.add
                      local.tee $l48
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l16
                      f32.load offset=32
                      local.get $l17
                      f32.load offset=32
                      f32.add
                      local.get $l10
                      f32.load offset=32
                      local.get $l21
                      f32.load offset=32
                      f32.add
                      f32.add
                      local.tee $l50
                      local.get $l10
                      f32.load offset=16
                      local.get $l21
                      f32.load offset=16
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l44
                      f32.mul
                      f32.add
                      local.get $l44
                      f32.mul
                      local.get $l16
                      f32.load offset=44
                      local.get $l17
                      f32.load offset=44
                      f32.add
                      local.get $l10
                      f32.load offset=44
                      local.get $l21
                      f32.load offset=44
                      f32.add
                      f32.add
                      local.tee $l45
                      local.get $l44
                      f32.mul
                      local.get $l16
                      f32.load offset=52
                      local.get $l17
                      f32.load offset=52
                      f32.add
                      local.get $l10
                      f32.load offset=52
                      local.get $l21
                      f32.load offset=52
                      f32.add
                      f32.add
                      local.tee $l46
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l16
                      f32.load offset=28
                      local.get $l17
                      f32.load offset=28
                      f32.add
                      local.get $l10
                      f32.load offset=28
                      local.get $l21
                      f32.load offset=28
                      f32.add
                      f32.add
                      local.tee $l47
                      local.get $l10
                      f32.load offset=12
                      local.get $l21
                      f32.load offset=12
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l44
                      f32.mul
                      f32.add
                      local.get $l44
                      f32.mul
                      local.get $l16
                      f32.load offset=36
                      local.get $l17
                      f32.load offset=36
                      f32.add
                      local.get $l10
                      f32.load offset=36
                      local.get $l21
                      f32.load offset=36
                      f32.add
                      f32.add
                      local.tee $l51
                      local.get $l44
                      f32.mul
                      local.get $l16
                      f32.load offset=48
                      local.get $l17
                      f32.load offset=48
                      f32.add
                      local.get $l10
                      f32.load offset=48
                      local.get $l21
                      f32.load offset=48
                      f32.add
                      f32.add
                      local.tee $l52
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l16
                      f32.load offset=24
                      local.get $l17
                      f32.load offset=24
                      f32.add
                      local.get $l10
                      f32.load offset=24
                      local.get $l21
                      f32.load offset=24
                      f32.add
                      f32.add
                      local.tee $l40
                      local.get $l19
                      f32.mul
                      f32.add
                      local.get $l19
                      f32.mul
                      local.get $l16
                      f32.load offset=60
                      local.get $l17
                      f32.load offset=60
                      f32.add
                      local.get $l10
                      f32.load offset=60
                      local.get $l21
                      f32.load offset=60
                      f32.add
                      f32.add
                      local.tee $l41
                      f32.add
                      f32.add
                      f32.add
                      f32.abs
                      local.tee $l42
                      local.get $l43
                      local.get $l16
                      f32.load offset=20
                      local.get $l17
                      f32.load offset=20
                      f32.add
                      local.get $l10
                      f32.load offset=20
                      local.get $l21
                      f32.load offset=20
                      f32.add
                      f32.add
                      f32.mul
                      f32.gt
                      local.get $l49
                      local.get $l16
                      f32.load offset=8
                      local.get $l17
                      f32.load offset=8
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l19
                      f32.mul
                      local.get $l48
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l50
                      local.get $l16
                      f32.load offset=16
                      local.get $l17
                      f32.load offset=16
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l44
                      f32.mul
                      f32.add
                      local.get $l44
                      f32.mul
                      local.get $l45
                      local.get $l44
                      f32.mul
                      local.get $l46
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l47
                      local.get $l16
                      f32.load offset=12
                      local.get $l17
                      f32.load offset=12
                      f32.add
                      f32.const 0x1p-1 (;=0.5;)
                      f32.mul
                      local.tee $l44
                      f32.mul
                      f32.add
                      local.get $l44
                      f32.mul
                      local.get $l51
                      local.get $l44
                      f32.mul
                      local.get $l52
                      f32.add
                      local.tee $l44
                      local.get $l44
                      f32.add
                      local.get $l40
                      local.get $l19
                      f32.mul
                      f32.add
                      local.get $l19
                      f32.mul
                      local.get $l41
                      f32.add
                      f32.add
                      f32.add
                      f32.abs
                      local.get $l42
                      f32.const 0x1.71eb86p-1 (;=0.7225;)
                      f32.mul
                      f32.lt
                      i32.and
                      local.set $l10
                      br $B53
                    end
                    local.get $l24
                    local.get $l38
                    i32.add
                    local.set $l31
                    i32.const 0
                    local.set $l16
                    i32.const 7
                    local.set $l17
                    loop $L57
                      block $B58
                        local.get $l20
                        local.get $l16
                        i32.shr_u
                        i32.const 1
                        i32.and
                        i32.eqz
                        br_if $B58
                        local.get $l31
                        local.get $l16
                        i32.const 1
                        i32.and
                        i32.add
                        local.get $l16
                        i32.const 1
                        i32.shr_u
                        i32.const 1
                        i32.and
                        local.get $p5
                        i32.add
                        local.get $l16
                        i32.const 2
                        i32.shr_u
                        local.get $l14
                        i32.add
                        local.get $p7
                        i32.mul
                        i32.add
                        local.tee $l10
                        local.get $p7
                        i32.mul
                        i32.add
                        i32.load8_u
                        local.tee $l21
                        i32.const 255
                        i32.eq
                        br_if $B58
                        local.get $l22
                        local.get $l27
                        local.get $l10
                        i32.const 2
                        i32.shl
                        i32.add
                        i32.load
                        i32.const 6
                        i32.shl
                        i32.add
                        local.get $l21
                        i32.const 6
                        i32.shl
                        i32.add
                        i32.const -60
                        i32.add
                        i32.load8_u
                        local.get $l17
                        i32.shr_u
                        i32.const 1
                        i32.and
                        i32.eqz
                        br_if $B58
                        i32.const 0
                        local.set $l10
                        br $B53
                      end
                      local.get $l17
                      i32.const -1
                      i32.add
                      local.set $l17
                      i32.const 1
                      local.set $l10
                      local.get $l16
                      i32.const 1
                      i32.add
                      local.tee $l16
                      i32.const 8
                      i32.ne
                      br_if $L57
                    end
                  end
                  local.get $l20
                  i32.const 20
                  i32.mul
                  local.get $l10
                  i32.const 10
                  i32.mul
                  i32.add
                  local.tee $l17
                  i32.load16_u offset=37616
                  local.tee $l16
                  i32.eqz
                  br_if $B52
                  local.get $l17
                  i32.const 37618
                  i32.add
                  local.set $l10
                  local.get $l24
                  local.get $l38
                  i32.add
                  local.set $l20
                  local.get $p0
                  local.get $l15
                  i32.const 36
                  i32.mul
                  i32.add
                  local.set $l17
                  loop $L59
                    block $B60
                      local.get $l15
                      local.get $p1
                      i32.ge_u
                      br_if $B60
                      local.get $l17
                      local.get $l22
                      local.get $l27
                      local.get $l16
                      i32.const 9
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $p5
                      i32.add
                      local.get $l16
                      i32.const 10
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $l14
                      i32.add
                      local.get $p7
                      i32.mul
                      i32.add
                      local.tee $l21
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      i32.const 6
                      i32.shl
                      i32.add
                      local.get $l20
                      local.get $l16
                      i32.const 8
                      i32.shr_u
                      i32.const 1
                      i32.and
                      i32.add
                      local.get $l21
                      local.get $p7
                      i32.mul
                      i32.add
                      i32.load8_u
                      i32.const 6
                      i32.shl
                      i32.add
                      local.tee $l21
                      i32.const -56
                      i32.add
                      f32.load
                      local.get $l13
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 4
                      i32.add
                      local.get $l21
                      i32.const -52
                      i32.add
                      f32.load
                      local.get $l12
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 8
                      i32.add
                      local.get $l21
                      i32.const -48
                      i32.add
                      f32.load
                      local.get $l11
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 12
                      i32.add
                      local.get $l13
                      local.get $l22
                      local.get $l27
                      local.get $l16
                      i32.const 5
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $p5
                      i32.add
                      local.get $l16
                      i32.const 6
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $l14
                      i32.add
                      local.get $p7
                      i32.mul
                      i32.add
                      local.tee $l21
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      i32.const 6
                      i32.shl
                      i32.add
                      local.get $l20
                      local.get $l16
                      i32.const 4
                      i32.shr_u
                      i32.const 1
                      i32.and
                      i32.add
                      local.get $l21
                      local.get $p7
                      i32.mul
                      i32.add
                      i32.load8_u
                      i32.const 6
                      i32.shl
                      i32.add
                      local.tee $l21
                      i32.const -56
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 16
                      i32.add
                      local.get $l12
                      local.get $l21
                      i32.const -52
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 20
                      i32.add
                      local.get $l11
                      local.get $l21
                      i32.const -48
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 24
                      i32.add
                      local.get $l13
                      local.get $l22
                      local.get $l27
                      local.get $l16
                      i32.const 1
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $p5
                      i32.add
                      local.get $l16
                      i32.const 2
                      i32.shr_u
                      i32.const 1
                      i32.and
                      local.get $l14
                      i32.add
                      local.get $p7
                      i32.mul
                      i32.add
                      local.tee $l21
                      i32.const 2
                      i32.shl
                      i32.add
                      i32.load
                      i32.const 6
                      i32.shl
                      i32.add
                      local.get $l20
                      local.get $l16
                      i32.const 1
                      i32.and
                      i32.add
                      local.get $l21
                      local.get $p7
                      i32.mul
                      i32.add
                      i32.load8_u
                      i32.const 6
                      i32.shl
                      i32.add
                      local.tee $l16
                      i32.const -56
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 28
                      i32.add
                      local.get $l12
                      local.get $l16
                      i32.const -52
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                      local.get $l17
                      i32.const 32
                      i32.add
                      local.get $l11
                      local.get $l16
                      i32.const -48
                      i32.add
                      f32.load
                      f32.add
                      f32.store
                    end
                    local.get $l17
                    i32.const 36
                    i32.add
                    local.set $l17
                    local.get $l15
                    i32.const 1
                    i32.add
                    local.set $l15
                    local.get $l10
                    i32.load16_u
                    local.set $l16
                    local.get $l10
                    i32.const 2
                    i32.add
                    local.set $l10
                    local.get $l16
                    br_if $L59
                  end
                end
                local.get $l26
                local.get $l28
                i32.ne
                br_if $L51
              end
            end
            local.get $p5
            i32.const 1
            i32.add
            local.tee $p5
            local.get $l28
            i32.ne
            br_if $L49
          end
          local.get $l14
          i32.const 1
          i32.add
          local.tee $l14
          local.get $l28
          i32.ne
          br_if $L48
        end
        local.get $l9
        i32.load offset=108
        local.tee $l25
        i32.eqz
        br_if $B40
      end
      local.get $l25
      i32.const 2
      i32.shl
      local.get $l9
      i32.const 12
      i32.add
      i32.add
      i32.const -4
      i32.add
      local.set $l16
      loop $L61
        local.get $l16
        i32.load
        i32.const 0
        i32.load offset=37596
        call_indirect $T0 (type $t1)
        local.get $l16
        i32.const -4
        i32.add
        local.set $l16
        local.get $l25
        i32.const -1
        i32.add
        local.tee $l25
        br_if $L61
      end
    end
    local.get $l9
    i32.const 384
    i32.add
    global.set $g0
    local.get $l15)
  (func $f17 (type $t16) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (param $p4 i32) (param $p5 i32) (param $p6 i32) (param $p7 i32) (param $p8 f32) (param $p9 i32) (param $p10 i32)
    (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 f32) (local $l15 i32) (local $l16 i32) (local $l17 f32) (local $l18 i32) (local $l19 f32) (local $l20 f32) (local $l21 f32) (local $l22 f32) (local $l23 f32) (local $l24 f32) (local $l25 f32) (local $l26 f32) (local $l27 f32) (local $l28 f32) (local $l29 f32) (local $l30 f32) (local $l31 f32) (local $l32 f32) (local $l33 f32) (local $l34 f32) (local $l35 i32) (local $l36 f32) (local $l37 f32) (local $l38 f32) (local $l39 i32) (local $l40 f32) (local $l41 f32) (local $l42 i32) (local $l43 i32) (local $l44 f32) (local $l45 f32) (local $l46 i32) (local $l47 i32) (local $l48 i32) (local $l49 i32) (local $l50 i32) (local $l51 i32)
    block $B0
      local.get $p4
      i32.eqz
      br_if $B0
      local.get $p6
      i32.const 2
      i32.shr_u
      local.set $l11
      local.get $p10
      i32.const 2
      i32.and
      local.set $l12
      local.get $p7
      i32.const -3
      i32.add
      local.set $p10
      local.get $p7
      i32.const 1
      i32.shl
      local.set $l13
      local.get $p8
      local.get $p8
      f32.add
      local.set $l14
      i32.const 0
      local.set $l15
      loop $L1
        block $B2
          block $B3
            local.get $p8
            local.get $p5
            local.get $p3
            local.get $l15
            i32.const 2
            i32.shl
            i32.add
            local.tee $p6
            i32.const 4
            i32.add
            i32.load
            local.get $l11
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $l16
            f32.load offset=8
            local.tee $l17
            local.get $p5
            local.get $p6
            i32.load
            local.get $l11
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $l18
            f32.load offset=8
            local.tee $l19
            f32.sub
            local.tee $l20
            local.get $l20
            f32.mul
            local.get $l16
            f32.load
            local.tee $l21
            local.get $l18
            f32.load
            local.tee $l22
            f32.sub
            local.tee $l23
            local.get $l23
            f32.mul
            local.get $l16
            f32.load offset=4
            local.tee $l24
            local.get $l18
            f32.load offset=4
            local.tee $l25
            f32.sub
            local.tee $l26
            local.get $l26
            f32.mul
            f32.add
            f32.add
            local.tee $l27
            local.get $p5
            local.get $p6
            i32.const 8
            i32.add
            i32.load
            local.get $l11
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $p6
            f32.load offset=8
            local.tee $l28
            local.get $l19
            f32.sub
            local.tee $l29
            local.get $l29
            f32.mul
            local.get $p6
            f32.load
            local.tee $l30
            local.get $l22
            f32.sub
            local.tee $l31
            local.get $l31
            f32.mul
            local.get $p6
            f32.load offset=4
            local.tee $l32
            local.get $l25
            f32.sub
            local.tee $l33
            local.get $l33
            f32.mul
            f32.add
            f32.add
            local.tee $l34
            local.get $l27
            local.get $l34
            f32.gt
            select
            local.tee $l27
            local.get $l28
            local.get $l17
            f32.sub
            local.tee $l17
            local.get $l17
            f32.mul
            local.get $l30
            local.get $l21
            f32.sub
            local.tee $l17
            local.get $l17
            f32.mul
            local.get $l32
            local.get $l24
            f32.sub
            local.tee $l17
            local.get $l17
            f32.mul
            f32.add
            f32.add
            local.tee $l17
            local.get $l27
            local.get $l17
            f32.gt
            select
            f32.sqrt
            f32.mul
            local.tee $l17
            local.get $l17
            f32.add
            local.tee $l17
            f32.abs
            f32.const 0x1p+31 (;=2.14748e+09;)
            f32.lt
            i32.eqz
            br_if $B3
            local.get $l17
            i32.trunc_f32_s
            local.set $p6
            br $B2
          end
          i32.const -2147483648
          local.set $p6
        end
        block $B4
          local.get $l23
          local.get $l33
          f32.mul
          local.get $l26
          local.get $l31
          f32.mul
          f32.sub
          local.tee $l17
          local.get $l17
          f32.mul
          local.get $l26
          local.get $l29
          f32.mul
          local.get $l20
          local.get $l33
          f32.mul
          f32.sub
          local.tee $l21
          local.get $l21
          f32.mul
          local.get $l20
          local.get $l31
          f32.mul
          local.get $l23
          local.get $l29
          f32.mul
          f32.sub
          local.tee $l24
          local.get $l24
          f32.mul
          f32.add
          f32.add
          local.tee $l28
          f32.const 0x0p+0 (;=0;)
          f32.eq
          br_if $B4
          f32.const 0x1p+0 (;=1;)
          local.set $l27
          block $B5
            local.get $p6
            i32.const 1
            local.get $p6
            i32.const 1
            i32.gt_s
            select
            local.tee $p6
            local.get $l13
            local.get $p6
            local.get $l13
            i32.lt_s
            select
            local.tee $l35
            i32.const 2
            i32.lt_s
            br_if $B5
            f32.const 0x1p+0 (;=1;)
            local.get $l35
            f32.convert_i32_u
            f32.div
            local.set $l27
          end
          local.get $p7
          i32.const 0
          i32.lt_s
          br_if $B4
          local.get $l19
          local.get $p9
          f32.load offset=8
          f32.sub
          local.set $l36
          local.get $l25
          local.get $p9
          f32.load offset=4
          f32.sub
          local.set $l37
          local.get $l22
          local.get $p9
          f32.load
          f32.sub
          local.set $l38
          local.get $l35
          i32.const 0
          local.get $l35
          i32.const 0
          i32.gt_s
          select
          local.set $l39
          block $B6
            local.get $p2
            i32.eqz
            br_if $B6
            local.get $l17
            f32.const 0x1p+0 (;=1;)
            local.get $l28
            f32.sqrt
            local.tee $l19
            f32.div
            local.tee $l22
            f32.mul
            local.tee $l28
            local.get $l19
            local.get $l35
            i32.const 1
            i32.add
            local.get $l35
            i32.const 2
            i32.add
            i32.mul
            f32.convert_i32_s
            f32.div
            local.tee $l19
            f32.mul
            local.set $l30
            local.get $l24
            local.get $l22
            f32.mul
            local.tee $l24
            local.get $l19
            f32.mul
            local.set $l40
            local.get $l21
            local.get $l22
            f32.mul
            local.tee $l21
            local.get $l21
            local.get $l19
            f32.mul
            f32.mul
            local.set $l41
            i32.const 0
            local.set $l42
            loop $L7
              local.get $l35
              i32.const 0
              local.get $l35
              i32.const 0
              i32.gt_s
              select
              i32.const 1
              i32.add
              local.set $l43
              local.get $l27
              local.get $l42
              f32.convert_i32_u
              f32.mul
              local.tee $l22
              local.get $l20
              f32.mul
              local.get $l36
              f32.add
              local.set $l34
              local.get $l22
              local.get $l26
              f32.mul
              local.get $l37
              f32.add
              local.set $l44
              local.get $l22
              local.get $l23
              f32.mul
              local.get $l38
              f32.add
              local.set $l45
              i32.const 0
              local.set $l16
              loop $L8
                block $B9
                  block $B10
                    local.get $l14
                    local.get $l27
                    local.get $l16
                    f32.convert_i32_u
                    f32.mul
                    local.tee $l22
                    local.get $l33
                    f32.mul
                    local.get $l44
                    f32.add
                    local.tee $l25
                    f32.mul
                    local.tee $l17
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B10
                    local.get $l17
                    i32.trunc_f32_s
                    local.set $l18
                    br $B9
                  end
                  i32.const -2147483648
                  local.set $l18
                end
                local.get $l18
                i32.const 1
                i32.shr_s
                local.tee $p6
                local.get $p10
                local.get $p6
                local.get $p10
                i32.lt_u
                select
                local.set $l46
                block $B11
                  block $B12
                    local.get $l14
                    local.get $l22
                    local.get $l29
                    f32.mul
                    local.get $l34
                    f32.add
                    local.tee $l17
                    f32.mul
                    local.tee $l32
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B12
                    local.get $l32
                    i32.trunc_f32_s
                    local.set $l47
                    br $B11
                  end
                  i32.const -2147483648
                  local.set $l47
                end
                local.get $l46
                local.get $l47
                i32.const 1
                i32.shr_s
                local.tee $p6
                local.get $p10
                local.get $p6
                local.get $p10
                i32.lt_u
                select
                local.tee $l48
                i32.const 1
                i32.add
                local.get $p7
                i32.mul
                i32.add
                i32.const 1
                i32.add
                local.tee $p6
                local.get $p7
                i32.mul
                local.set $l49
                block $B13
                  block $B14
                    local.get $l14
                    local.get $l22
                    local.get $l31
                    f32.mul
                    local.get $l45
                    f32.add
                    local.tee $l22
                    f32.mul
                    local.tee $l32
                    f32.abs
                    f32.const 0x1p+31 (;=2.14748e+09;)
                    f32.lt
                    i32.eqz
                    br_if $B14
                    local.get $l32
                    i32.trunc_f32_s
                    local.set $l50
                    br $B13
                  end
                  i32.const -2147483648
                  local.set $l50
                end
                local.get $p2
                local.get $p1
                local.get $p6
                i32.const 2
                i32.shl
                i32.add
                i32.load
                i32.const 6
                i32.shl
                i32.add
                local.get $p0
                local.get $l50
                i32.const 1
                i32.shr_s
                local.tee $p6
                local.get $p10
                local.get $p6
                local.get $p10
                i32.lt_u
                select
                local.tee $l51
                i32.add
                local.get $l49
                i32.add
                i32.const 1
                i32.add
                i32.load8_u
                i32.const 6
                i32.shl
                i32.add
                local.tee $p6
                i32.const -64
                i32.add
                local.get $l46
                i32.const 10
                i32.shl
                local.get $l51
                i32.const 20
                i32.shl
                i32.or
                local.get $l48
                i32.or
                i32.store
                local.get $p6
                i32.const -60
                i32.add
                local.tee $l46
                local.get $l46
                i32.load8_u
                i32.const 1
                local.get $l50
                i32.const 1
                i32.and
                local.get $l18
                i32.const 1
                i32.shl
                i32.const 2
                i32.and
                i32.or
                local.get $l47
                i32.const 2
                i32.shl
                i32.const 4
                i32.and
                i32.or
                i32.shl
                i32.or
                i32.store8
                local.get $p6
                i32.const -56
                i32.add
                local.tee $l18
                local.get $l22
                local.get $l19
                f32.mul
                local.get $l18
                f32.load
                f32.add
                f32.store
                local.get $p6
                i32.const -52
                i32.add
                local.tee $l18
                local.get $l25
                local.get $l19
                f32.mul
                local.get $l18
                f32.load
                f32.add
                f32.store
                local.get $p6
                i32.const -48
                i32.add
                local.tee $l18
                local.get $l17
                local.get $l19
                f32.mul
                local.get $l18
                f32.load
                f32.add
                f32.store
                local.get $p6
                i32.const -44
                i32.add
                local.tee $l18
                local.get $l19
                local.get $l18
                f32.load
                f32.add
                f32.store
                block $B15
                  local.get $l12
                  i32.eqz
                  br_if $B15
                  local.get $p6
                  i32.const -40
                  i32.add
                  local.tee $l18
                  local.get $l41
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -36
                  i32.add
                  local.tee $l18
                  local.get $l24
                  local.get $l40
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -32
                  i32.add
                  local.tee $l18
                  local.get $l28
                  local.get $l30
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -28
                  i32.add
                  local.tee $l18
                  local.get $l21
                  local.get $l40
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -24
                  i32.add
                  local.tee $l18
                  local.get $l21
                  local.get $l30
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -20
                  i32.add
                  local.tee $l18
                  local.get $l24
                  local.get $l30
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -16
                  i32.add
                  local.tee $l18
                  local.get $l21
                  local.get $l19
                  local.get $l28
                  local.get $l17
                  f32.mul
                  local.get $l21
                  local.get $l22
                  f32.mul
                  local.get $l24
                  local.get $l25
                  f32.mul
                  f32.add
                  f32.add
                  local.tee $l25
                  f32.neg
                  f32.mul
                  local.tee $l22
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -12
                  i32.add
                  local.tee $l18
                  local.get $l24
                  local.get $l22
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -8
                  i32.add
                  local.tee $l18
                  local.get $l28
                  local.get $l22
                  f32.mul
                  local.get $l18
                  f32.load
                  f32.add
                  f32.store
                  local.get $p6
                  i32.const -4
                  i32.add
                  local.tee $p6
                  local.get $p6
                  f32.load
                  local.get $l25
                  local.get $l22
                  f32.mul
                  f32.sub
                  f32.store
                end
                local.get $l43
                local.get $l16
                i32.const 1
                i32.add
                local.tee $l16
                i32.ne
                br_if $L8
              end
              local.get $l35
              i32.const -1
              i32.add
              local.set $l35
              local.get $l42
              local.get $l39
              i32.eq
              local.set $p6
              local.get $l42
              i32.const 1
              i32.add
              local.set $l42
              local.get $p6
              i32.eqz
              br_if $L7
              br $B4
            end
          end
          i32.const 0
          local.set $l47
          loop $L16
            local.get $l35
            i32.const 0
            local.get $l35
            i32.const 0
            i32.gt_s
            select
            i32.const 1
            i32.add
            local.set $l46
            local.get $l27
            local.get $l47
            f32.convert_i32_u
            f32.mul
            local.tee $l19
            local.get $l20
            f32.mul
            local.get $l36
            f32.add
            local.set $l25
            local.get $l19
            local.get $l26
            f32.mul
            local.get $l37
            f32.add
            local.set $l17
            local.get $l19
            local.get $l23
            f32.mul
            local.get $l38
            f32.add
            local.set $l21
            i32.const 0
            local.set $p6
            loop $L17
              block $B18
                block $B19
                  local.get $l14
                  local.get $l27
                  local.get $p6
                  f32.convert_i32_u
                  f32.mul
                  local.tee $l19
                  local.get $l33
                  f32.mul
                  local.get $l17
                  f32.add
                  f32.mul
                  local.tee $l22
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B19
                  local.get $l22
                  i32.trunc_f32_s
                  local.set $l16
                  br $B18
                end
                i32.const -2147483648
                local.set $l16
              end
              local.get $l16
              i32.const 1
              i32.shr_s
              local.tee $l16
              local.get $p10
              local.get $l16
              local.get $p10
              i32.lt_u
              select
              local.set $l16
              block $B20
                block $B21
                  local.get $l14
                  local.get $l19
                  local.get $l29
                  f32.mul
                  local.get $l25
                  f32.add
                  f32.mul
                  local.tee $l22
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B21
                  local.get $l22
                  i32.trunc_f32_s
                  local.set $l18
                  br $B20
                end
                i32.const -2147483648
                local.set $l18
              end
              local.get $l16
              local.get $l18
              i32.const 1
              i32.shr_s
              local.tee $l18
              local.get $p10
              local.get $l18
              local.get $p10
              i32.lt_u
              select
              i32.const 1
              i32.add
              local.get $p7
              i32.mul
              i32.add
              i32.const 1
              i32.add
              local.get $p7
              i32.mul
              local.set $l16
              block $B22
                block $B23
                  local.get $l14
                  local.get $l19
                  local.get $l31
                  f32.mul
                  local.get $l21
                  f32.add
                  f32.mul
                  local.tee $l19
                  f32.abs
                  f32.const 0x1p+31 (;=2.14748e+09;)
                  f32.lt
                  i32.eqz
                  br_if $B23
                  local.get $l19
                  i32.trunc_f32_s
                  local.set $l18
                  br $B22
                end
                i32.const -2147483648
                local.set $l18
              end
              local.get $p0
              local.get $l18
              i32.const 1
              i32.shr_s
              local.tee $l18
              local.get $p10
              local.get $l18
              local.get $p10
              i32.lt_u
              select
              i32.add
              local.get $l16
              i32.add
              i32.const 1
              i32.add
              i32.const 1
              i32.store8
              local.get $l46
              local.get $p6
              i32.const 1
              i32.add
              local.tee $p6
              i32.ne
              br_if $L17
            end
            local.get $l35
            i32.const -1
            i32.add
            local.set $l35
            local.get $l47
            local.get $l39
            i32.eq
            local.set $p6
            local.get $l47
            i32.const 1
            i32.add
            local.set $l47
            local.get $p6
            i32.eqz
            br_if $L16
          end
        end
        local.get $l15
        i32.const 3
        i32.add
        local.tee $l15
        local.get $p4
        i32.lt_u
        br_if $L1
      end
    end)
  (func $f18 (type $t2)
    (local $l0 i32) (local $l1 i32) (local $l2 i32) (local $l3 i32) (local $l4 i32) (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 i32) (local $l15 i32)
    global.get $g0
    i32.const 256
    i32.sub
    local.tee $l0
    global.set $g0
    i32.const 0
    local.set $l1
    local.get $l0
    i32.const 0
    i32.const 256
    call $f23
    local.set $l2
    i32.const 37040
    local.set $l0
    loop $L0
      local.get $l0
      i32.load8_u
      local.tee $l3
      i32.const 20
      i32.mul
      local.tee $l4
      local.get $l0
      i32.const 10
      i32.add
      i32.load16_u
      i32.store16 offset=37624
      local.get $l4
      local.get $l0
      i32.const 2
      i32.add
      local.tee $l5
      i64.load align=2
      i64.store offset=37616 align=2
      local.get $l3
      local.get $l0
      i32.const 1
      i32.add
      i32.load8_u
      i32.store8 offset=42736
      local.get $l2
      local.get $l3
      i32.add
      i32.const 1
      i32.store8
      local.get $l4
      local.get $l5
      local.get $l0
      i32.const 12
      i32.add
      i32.const 1
      local.get $l1
      i32.shl
      i32.const 11534751
      i32.and
      select
      local.tee $l3
      i64.load align=2
      i64.store offset=37626 align=2
      local.get $l4
      local.get $l3
      i32.load16_u offset=8
      i32.store16 offset=37634
      local.get $l0
      i32.const 22
      i32.add
      local.set $l0
      local.get $l1
      i32.const 1
      i32.add
      local.tee $l1
      i32.const 24
      i32.ne
      br_if $L0
    end
    i32.const 0
    local.set $l6
    loop $L1
      i32.const 37618
      local.set $l7
      i32.const 0
      local.set $l5
      loop $L2
        block $B3
          local.get $l2
          local.get $l5
          i32.add
          local.tee $l8
          i32.load8_u
          i32.const 1
          i32.ne
          br_if $B3
          local.get $l5
          i32.const 20
          i32.mul
          i32.const 37616
          i32.add
          local.set $l9
          i32.const 0
          local.set $l0
          i32.const 1
          local.set $l10
          local.get $l5
          i32.const 42736
          i32.add
          local.set $l11
          loop $L4
            local.get $l0
            i32.const 3
            i32.shl
            i32.const 37024
            i32.add
            local.set $l4
            i32.const 0
            local.set $l0
            i32.const 0
            local.set $l1
            loop $L5
              local.get $l1
              local.get $l5
              local.get $l0
              i32.shr_u
              i32.const 1
              i32.and
              local.get $l4
              local.get $l0
              i32.add
              i32.load8_u
              i32.shl
              i32.or
              local.set $l1
              local.get $l0
              i32.const 1
              i32.add
              local.tee $l0
              i32.const 8
              i32.ne
              br_if $L5
            end
            block $B6
              local.get $l2
              local.get $l1
              i32.const 255
              i32.and
              local.tee $l12
              i32.add
              local.tee $l13
              i32.load8_u
              br_if $B6
              local.get $l12
              i32.const 20
              i32.mul
              i32.const 37616
              i32.add
              local.set $l14
              i32.const 0
              local.set $l0
              i32.const 1
              local.set $l15
              loop $L7
                block $B8
                  local.get $l9
                  local.get $l0
                  i32.const 10
                  i32.mul
                  local.tee $l3
                  i32.add
                  i32.load16_u
                  local.tee $l0
                  i32.eqz
                  br_if $B8
                  local.get $l7
                  local.get $l3
                  i32.add
                  local.set $l1
                  local.get $l14
                  local.get $l3
                  i32.add
                  local.set $l3
                  loop $L9
                    local.get $l3
                    local.get $l4
                    local.get $l0
                    i32.const 4
                    i32.shr_u
                    i32.const 15
                    i32.and
                    i32.add
                    i32.load8_u
                    i32.const 4
                    i32.shl
                    local.get $l4
                    local.get $l0
                    i32.const 8
                    i32.shr_u
                    i32.const 15
                    i32.and
                    i32.add
                    i32.load8_u
                    i32.const 8
                    i32.shl
                    i32.or
                    local.get $l4
                    local.get $l0
                    i32.const 15
                    i32.and
                    i32.add
                    i32.load8_u
                    i32.or
                    i32.store16
                    local.get $l3
                    i32.const 2
                    i32.add
                    local.set $l3
                    local.get $l1
                    i32.load16_u
                    local.set $l0
                    local.get $l1
                    i32.const 2
                    i32.add
                    local.set $l1
                    local.get $l0
                    br_if $L9
                  end
                end
                i32.const 1
                local.set $l0
                local.get $l15
                i32.const 1
                i32.and
                local.set $l1
                i32.const 0
                local.set $l15
                local.get $l1
                br_if $L7
              end
              local.get $l13
              i32.const 1
              i32.store8
              local.get $l12
              local.get $l11
              i32.load8_u
              i32.store8 offset=42736
            end
            i32.const 1
            local.set $l0
            local.get $l10
            i32.const 1
            i32.and
            local.set $l4
            i32.const 0
            local.set $l10
            local.get $l4
            br_if $L4
          end
          local.get $l8
          i32.const 2
          i32.store8
        end
        local.get $l7
        i32.const 20
        i32.add
        local.set $l7
        local.get $l5
        i32.const 1
        i32.add
        local.tee $l5
        i32.const 256
        i32.ne
        br_if $L2
      end
      local.get $l6
      i32.const 1
      i32.add
      local.tee $l6
      i32.const 3
      i32.ne
      br_if $L1
    end
    i32.const 0
    local.set $l3
    i32.const 37616
    local.set $l5
    loop $L10
      i32.const -1
      local.set $l4
      local.get $l5
      local.set $l0
      loop $L11
        local.get $l4
        i32.const 1
        i32.add
        local.set $l4
        local.get $l0
        i32.load16_u
        local.set $l1
        local.get $l0
        i32.const 2
        i32.add
        local.set $l0
        local.get $l1
        br_if $L11
      end
      local.get $l3
      local.get $l4
      i32.store8 offset=42992
      local.get $l5
      i32.const 20
      i32.add
      local.set $l5
      local.get $l3
      i32.const 1
      i32.add
      local.tee $l3
      i32.const 256
      i32.ne
      br_if $L10
    end
    local.get $l2
    i32.const 256
    i32.add
    global.set $g0)
  (func $meshopt_optimizeVertexFetchRemap (type $t17) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32) (result i32)
    (local $l4 i32)
    local.get $p0
    i32.const 255
    local.get $p3
    i32.const 2
    i32.shl
    call $f23
    local.set $l4
    i32.const 0
    local.set $p0
    block $B0
      local.get $p2
      i32.eqz
      br_if $B0
      loop $L1
        block $B2
          local.get $l4
          local.get $p1
          i32.load
          i32.const 2
          i32.shl
          i32.add
          local.tee $p3
          i32.load
          i32.const -1
          i32.ne
          br_if $B2
          local.get $p3
          local.get $p0
          i32.store
          local.get $p0
          i32.const 1
          i32.add
          local.set $p0
        end
        local.get $p1
        i32.const 4
        i32.add
        local.set $p1
        local.get $p2
        i32.const -1
        i32.add
        local.tee $p2
        br_if $L1
      end
    end
    local.get $p0)
  (func $meshopt_generatePositionRemap (type $t18) (param $p0 i32) (param $p1 i32) (param $p2 i32) (param $p3 i32)
    (local $l4 i32) (local $l5 i32) (local $l6 i32) (local $l7 i32) (local $l8 i32) (local $l9 i32) (local $l10 i32) (local $l11 i32) (local $l12 i32) (local $l13 i32) (local $l14 f32) (local $l15 f32) (local $l16 f32)
    global.get $g0
    i32.const 112
    i32.sub
    local.tee $l4
    global.set $g0
    local.get $l4
    i32.const 12
    i32.add
    i32.const 0
    i32.const 100
    call $f23
    drop
    local.get $p2
    i32.const 2
    i32.shr_u
    local.get $p2
    i32.add
    local.set $l5
    i32.const 1
    local.set $l6
    loop $L0
      local.get $l6
      local.tee $l7
      i32.const 1
      i32.shl
      local.set $l6
      local.get $l7
      local.get $l5
      i32.lt_u
      br_if $L0
    end
    local.get $l4
    i32.const -1
    local.get $l7
    i32.const 2
    i32.shl
    local.tee $l6
    local.get $l7
    i32.const 1073741823
    i32.gt_u
    select
    i32.const 0
    i32.load offset=37592
    call_indirect $T0 (type $t0)
    local.tee $l5
    i32.store offset=12
    local.get $l5
    i32.const 255
    local.get $l6
    call $f23
    local.set $l8
    block $B1
      local.get $p2
      i32.eqz
      br_if $B1
      local.get $p3
      i32.const 2
      i32.shr_u
      local.set $l9
      local.get $l7
      i32.const -1
      i32.add
      local.set $l10
      i32.const 0
      local.set $l11
      loop $L2
        block $B3
          block $B4
            local.get $l8
            i32.const 0
            local.get $p1
            local.get $l11
            local.get $l9
            i32.mul
            i32.const 2
            i32.shl
            i32.add
            local.tee $l7
            i32.load offset=4
            local.tee $p3
            local.get $p3
            i32.const -2147483648
            i32.eq
            select
            local.tee $l6
            i32.const 17
            i32.shr_u
            local.get $l6
            i32.xor
            i32.const 19349663
            i32.mul
            i32.const 0
            local.get $l7
            i32.load
            local.tee $l12
            local.get $l12
            i32.const -2147483648
            i32.eq
            select
            local.tee $l6
            i32.const 17
            i32.shr_u
            local.get $l6
            i32.xor
            i32.const 73856093
            i32.mul
            i32.xor
            i32.const 0
            local.get $l7
            i32.load offset=8
            local.tee $l13
            local.get $l13
            i32.const -2147483648
            i32.eq
            select
            local.tee $l7
            i32.const 17
            i32.shr_u
            local.get $l7
            i32.xor
            i32.const 83492791
            i32.mul
            i32.xor
            local.get $l10
            i32.and
            local.tee $l5
            i32.const 2
            i32.shl
            i32.add
            local.tee $l7
            i32.load
            local.tee $l6
            i32.const -1
            i32.eq
            br_if $B4
            local.get $l13
            f32.reinterpret_i32
            local.set $l14
            local.get $p3
            f32.reinterpret_i32
            local.set $l15
            local.get $l12
            f32.reinterpret_i32
            local.set $l16
            i32.const 1
            local.set $p3
            loop $L5
              local.get $p3
              local.set $l7
              block $B6
                local.get $p1
                local.get $l6
                local.get $l9
                i32.mul
                i32.const 2
                i32.shl
                i32.add
                local.tee $p3
                f32.load
                local.get $l16
                f32.ne
                br_if $B6
                local.get $p3
                f32.load offset=4
                local.get $l15
                f32.ne
                br_if $B6
                local.get $p3
                f32.load offset=8
                local.get $l14
                f32.eq
                br_if $B3
              end
              local.get $l7
              i32.const 1
              i32.add
              local.set $p3
              local.get $l8
              local.get $l5
              local.get $l7
              i32.add
              local.get $l10
              i32.and
              local.tee $l5
              i32.const 2
              i32.shl
              i32.add
              local.tee $l7
              i32.load
              local.tee $l6
              i32.const -1
              i32.ne
              br_if $L5
            end
          end
          local.get $l7
          local.get $l11
          i32.store
          local.get $l11
          local.set $l6
        end
        local.get $p0
        local.get $l11
        i32.const 2
        i32.shl
        i32.add
        local.get $l6
        i32.store
        local.get $l11
        i32.const 1
        i32.add
        local.tee $l11
        local.get $p2
        i32.ne
        br_if $L2
      end
    end
    local.get $l4
    i32.load offset=12
    i32.const 0
    i32.load offset=37596
    call_indirect $T0 (type $t1)
    local.get $l4
    i32.const 112
    i32.add
    global.set $g0)
  (func $sbrk (type $t0) (param $p0 i32) (result i32)
    (local $l1 i32) (local $l2 i32) (local $l3 i32)
    i32.const 0
    i32.const 0
    i32.load offset=37600
    local.tee $l1
    local.get $p0
    i32.const 3
    i32.add
    i32.const -4
    i32.and
    i32.add
    local.tee $p0
    i32.store offset=37600
    block $B0
      block $B1
        local.get $p0
        memory.size
        i32.const 16
        i32.shl
        local.tee $l2
        i32.le_u
        br_if $B1
        i32.const -1
        local.set $l3
        local.get $p0
        local.get $l2
        i32.sub
        i32.const 65535
        i32.add
        i32.const 16
        i32.shr_u
        memory.grow
        i32.const -1
        i32.eq
        br_if $B0
      end
      local.get $l1
      local.set $l3
    end
    local.get $l3)
  (func $f22 (type $t19) (param $p0 i32) (param $p1 i32) (param $p2 i32) (result i32)
    (local $l3 i32)
    block $B0
      block $B1
        local.get $p1
        local.get $p0
        i32.or
        i32.const 3
        i32.and
        i32.eqz
        br_if $B1
        local.get $p0
        local.set $l3
        br $B0
      end
      block $B2
        block $B3
          local.get $p2
          i32.const 16
          i32.ge_u
          br_if $B3
          local.get $p0
          local.set $l3
          br $B2
        end
        local.get $p0
        local.set $l3
        loop $L4
          local.get $l3
          local.get $p1
          i32.load
          i32.store
          local.get $l3
          i32.const 4
          i32.add
          local.get $p1
          i32.const 4
          i32.add
          i32.load
          i32.store
          local.get $l3
          i32.const 8
          i32.add
          local.get $p1
          i32.const 8
          i32.add
          i32.load
          i32.store
          local.get $l3
          i32.const 12
          i32.add
          local.get $p1
          i32.const 12
          i32.add
          i32.load
          i32.store
          local.get $p1
          i32.const 16
          i32.add
          local.set $p1
          local.get $l3
          i32.const 16
          i32.add
          local.set $l3
          local.get $p2
          i32.const -16
          i32.add
          local.tee $p2
          i32.const 15
          i32.gt_u
          br_if $L4
        end
      end
      local.get $p2
      i32.const 4
      i32.lt_u
      br_if $B0
      loop $L5
        local.get $l3
        local.get $p1
        i32.load
        i32.store
        local.get $p1
        i32.const 4
        i32.add
        local.set $p1
        local.get $l3
        i32.const 4
        i32.add
        local.set $l3
        local.get $p2
        i32.const -4
        i32.add
        local.tee $p2
        i32.const 3
        i32.gt_u
        br_if $L5
      end
    end
    block $B6
      local.get $p2
      i32.eqz
      br_if $B6
      loop $L7
        local.get $l3
        local.get $p1
        i32.load8_u
        i32.store8
        local.get $l3
        i32.const 1
        i32.add
        local.set $l3
        local.get $p1
        i32.const 1
        i32.add
        local.set $p1
        local.get $p2
        i32.const -1
        i32.add
        local.tee $p2
        br_if $L7
      end
    end
    local.get $p0)
  (func $f23 (type $t19) (param $p0 i32) (param $p1 i32) (param $p2 i32) (result i32)
    (local $l3 i32) (local $l4 i32)
    block $B0
      block $B1
        local.get $p0
        i32.const 3
        i32.and
        i32.eqz
        br_if $B1
        local.get $p0
        local.set $l3
        br $B0
      end
      local.get $p1
      i32.const 255
      i32.and
      i32.const 16843009
      i32.mul
      local.set $l4
      block $B2
        block $B3
          local.get $p2
          i32.const 16
          i32.ge_u
          br_if $B3
          local.get $p0
          local.set $l3
          br $B2
        end
        local.get $p0
        local.set $l3
        loop $L4
          local.get $l3
          local.get $l4
          i32.store
          local.get $l3
          i32.const 12
          i32.add
          local.get $l4
          i32.store
          local.get $l3
          i32.const 8
          i32.add
          local.get $l4
          i32.store
          local.get $l3
          i32.const 4
          i32.add
          local.get $l4
          i32.store
          local.get $l3
          i32.const 16
          i32.add
          local.set $l3
          local.get $p2
          i32.const -16
          i32.add
          local.tee $p2
          i32.const 15
          i32.gt_u
          br_if $L4
        end
      end
      local.get $p2
      i32.const 4
      i32.lt_u
      br_if $B0
      loop $L5
        local.get $l3
        local.get $l4
        i32.store
        local.get $l3
        i32.const 4
        i32.add
        local.set $l3
        local.get $p2
        i32.const -4
        i32.add
        local.tee $p2
        i32.const 3
        i32.gt_u
        br_if $L5
      end
    end
    block $B6
      local.get $p2
      i32.eqz
      br_if $B6
      loop $L7
        local.get $l3
        local.get $p1
        i32.store8
        local.get $l3
        i32.const 1
        i32.add
        local.set $l3
        local.get $p2
        i32.const -1
        i32.add
        local.tee $p2
        br_if $L7
      end
    end
    local.get $p0)
  (func $f24 (type $t0) (param $p0 i32) (result i32)
    (local $l1 i32) (local $l2 i32) (local $l3 i32)
    i32.const 0
    i32.const 0
    i32.load offset=37600
    local.tee $l1
    local.get $p0
    i32.const 7
    i32.add
    i32.const -8
    i32.and
    i32.add
    local.tee $p0
    i32.store offset=37600
    block $B0
      block $B1
        local.get $p0
        memory.size
        i32.const 16
        i32.shl
        local.tee $l2
        i32.le_u
        br_if $B1
        i32.const -1
        local.set $l3
        local.get $p0
        local.get $l2
        i32.sub
        i32.const 65535
        i32.add
        i32.const 16
        i32.shr_u
        memory.grow
        i32.const -1
        i32.eq
        br_if $B0
      end
      local.get $l1
      local.set $l3
    end
    local.get $l3)
  (func $f25 (type $t1) (param $p0 i32)
    (local $l1 i32)
    i32.const 0
    local.get $p0
    i32.const 0
    i32.load offset=37600
    local.tee $l1
    i32.sub
    i32.const 3
    i32.add
    i32.const -4
    i32.and
    local.get $l1
    i32.add
    local.tee $p0
    i32.store offset=37600
    block $B0
      local.get $p0
      memory.size
      i32.const 16
      i32.shl
      local.tee $l1
      i32.le_u
      br_if $B0
      local.get $p0
      local.get $l1
      i32.sub
      i32.const 65535
      i32.add
      i32.const 16
      i32.shr_u
      memory.grow
      drop
    end)
  (func $f26 (type $t19) (param $p0 i32) (param $p1 i32) (param $p2 i32) (result i32)
    (local $l3 i32) (local $l4 i32) (local $l5 i32)
    i32.const 0
    local.set $l3
    block $B0
      local.get $p2
      i32.eqz
      br_if $B0
      block $B1
        loop $L2
          local.get $p0
          i32.load8_u
          local.tee $l4
          local.get $p1
          i32.load8_u
          local.tee $l5
          i32.ne
          br_if $B1
          local.get $p1
          i32.const 1
          i32.add
          local.set $p1
          local.get $p0
          i32.const 1
          i32.add
          local.set $p0
          local.get $p2
          i32.const -1
          i32.add
          local.tee $p2
          br_if $L2
          br $B0
        end
      end
      local.get $l4
      local.get $l5
      i32.sub
      local.set $l3
    end
    local.get $l3)
  (table $T0 3 3 funcref)
  (memory $memory 1)
  (global $g0 (mut i32) (i32.const 36864))
  (export "memory" (memory $memory))
  (export "__wasm_call_ctors" (func $__wasm_call_ctors))
  (export "meshopt_simplify" (func $meshopt_simplify))
  (export "meshopt_simplifyWithAttributes" (func $meshopt_simplifyWithAttributes))
  (export "meshopt_simplifyWithUpdate" (func $meshopt_simplifyWithUpdate))
  (export "meshopt_simplifySloppy" (func $meshopt_simplifySloppy))
  (export "meshopt_simplifyPrune" (func $meshopt_simplifyPrune))
  (export "meshopt_simplifyPoints" (func $meshopt_simplifyPoints))
  (export "meshopt_simplifyScale" (func $meshopt_simplifyScale))
  (export "meshopt_remesh" (func $meshopt_remesh))
  (export "meshopt_optimizeVertexFetchRemap" (func $meshopt_optimizeVertexFetchRemap))
  (export "meshopt_generatePositionRemap" (func $meshopt_generatePositionRemap))
  (export "sbrk" (func $sbrk))
  (elem $e0 (i32.const 1) func $f24 $f25)
  (data $d0 (i32.const 36864) "\ff\ff\7f\7f\ff\ff\7f\7f\ff\ff\7f\7f\ff\ff\7f\ff\ff\ff\7f\ff\ff\ff\7f\ff\00\00\00\00\00\00\00\00\01\00\00\00\02\00\00\00\00\00\00\00\01\00\00\00\01\00\00\00\02\00\00\00\00\00\00\00\00\00\00\00\01\01\01\01\01\01\00\01\00\00\01\01\00\00\01\00\00\01\00\00\00\01\01\01\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\01\01\01\01\01\01\00\01\00\00\00\01\01\01\00\00\01\01\00\00\00\00\00\01\00\00\00\00\00\01\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\02\03\06\07\00\01\04\05\04\00\06\02\05\01\07\03\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\06\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\07\00 \01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\0f\020\012\00\00\00\00\00\00\00!\03 \01\00\00\00\00\00\00\16\01$\01B\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\17\00$\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\18\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\19\01C\00\03\04\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\1b\014\010\04\00\00\00\00\00\00\10\04\13\00\00\00\00\00\00\00\1d\01C\02@\03\00\00\00\00\00\00 \03$\00\00\00\00\00\00\00\1e\01B\012\04A\03\00\00\00\002\01\00\00\00\00\00\00\00\00\1f\02\12\04\13\02\00\00\00\00\00\004\012\04\00\00\00\00\00\00<\01E\03$\03C\05#\04\00\00\00\00\00\00\00\00\00\00\00\00=\01S\02$\05P\03\00\00\00\00@\02\04\05 \03\00\00\00\00?\02%\03$\05\00\00\00\00\00\00C\02E\03\00\00\00\00\00\00i\015\00`\05\06\036\05\00\00\00\00\00\00\00\00\00\00\00\00k\01c\00\05\06e\03\00\00\00\00\05\03\00\00\00\00\00\00\00\00o\01e\03`\05\00\00\00\00\00\000\056\00\00\00\00\00\00\00~\00B\016\05\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\7f\00e\03\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\f0\02G\05F\07\00\00\00\00\00\00e\04g\05\00\00\00\00\00\00\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\ff\ff\7f\7f\ff\ff\7f\7f\ff\ff\7f\7f\ff\ff\7f\ff\ff\ff\7f\ff\ff\ff\7f\ff")
  (data $d1 (i32.const 37592) "\01\00\00\00\02\00\00\00\f0\a8\00\00"))
