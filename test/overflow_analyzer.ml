open OUnit2

let tests = "sample test" >::: [
    "2 + 2 = 4" >:: (fun _ -> assert_equal (2 + 2) 4) ;
  ]

let _ = run_test_tt_main tests
