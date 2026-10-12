(module
  (func (export "fac") (param $x i32) (result i32)
    (local $r i32)
    (local.set $r (i32.const 1))
    (block $done
      (loop $loop
        (br_if $done (i32.eqz (local.get $x)))
        (local.set $r (i32.mul (local.get $r) (local.get $x)))
        (local.set $x (i32.sub (local.get $x) (i32.const 1)))
        (br $loop)))
    (local.get $r)))
