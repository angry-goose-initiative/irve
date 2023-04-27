/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

//TODO move this integration test to under xrve_proc_macro

/* Imports */

use xrve_proc_macro::unstringify;

/* Tests */

#[test]
fn test_unstringify() {
    assert_eq!(unstringify!("0"), 0);
    assert_eq!(unstringify!("123"), 123);
    assert_eq!(unstringify!("1234567890"), 1234567890);
    assert_eq!(unstringify!("0.0"), 0.0);
    assert_eq!(unstringify!("100 + 1000"), 1100);
    assert_eq!(unstringify!("\"Hello World!\".to_lowercase()"), "hello world!");
    assert_eq!(unstringify!("\"Hello World!\".to_uppercase()"), "HELLO WORLD!");
    assert_eq!(unstringify!("\"Hello World!\".len()"), 12);
    assert_eq!(unstringify!("true && false"), false);
    //TODO add more checks
}

//TODO add more tests
