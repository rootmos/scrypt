#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <scrypt.h>

void display_byte_array (uint8_t* arr, size_t len) {
  size_t i;
  for (i=0; i<len; i+=1) {
    printf("%02x ", arr[i]);
    if (!((i + 1) % 16) || (i == (len - 1))) { printf("\n"); }
  }
}

uint8_t evaluate_result (uint8_t test_number, int status, uint8_t* exp, size_t exp_len, uint8_t* out, size_t out_len) {
  if (status || (memcmp(exp, out, exp_len) != 0)) {
    printf("failure test %d with status %d\nexpected: \n", test_number, status);
    display_byte_array(exp, exp_len);
    printf("output: \n");
    display_byte_array(out, out_len);
    printf("\n");
    return 0;
  }
  return 1;
}

char test_1 () {
  uint8_t res[64];
  int status;
  uint8_t exp[] = {
    0x77, 0xd6, 0x57, 0x62, 0x38, 0x65, 0x7b, 0x20, 0x3b, 0x19, 0xca, 0x42, 0xc1, 0x8a, 0x04, 0x97,
    0xf1, 0x6b, 0x48, 0x44, 0xe3, 0x07, 0x4a, 0xe8, 0xdf, 0xdf, 0xfa, 0x3f, 0xed, 0xe2, 0x14, 0x42,
    0xfc, 0xd0, 0x06, 0x9d, 0xed, 0x09, 0x48, 0xf8, 0x32, 0x6a, 0x75, 0x3a, 0x0f, 0xc8, 0x1f, 0x17,
    0xe8, 0xd3, 0xe0, 0xfb, 0x2e, 0x0d, 0x36, 0x28, 0xcf, 0x35, 0xe2, 0x0c, 0x38, 0xd1, 0x89, 0x06 };
  return evaluate_result(1,
    scrypt("", 0, "", 0, 16, 1, 1, res, sizeof(res)),
    exp, sizeof(exp), res, sizeof(res));
}

char test_2 () {
  uint8_t res[64];
  int status;
  uint8_t exp[] = {
    0xfd, 0xba, 0xbe, 0x1c, 0x9d, 0x34, 0x72, 0x00, 0x78, 0x56, 0xe7, 0x19, 0x0d, 0x01, 0xe9, 0xfe,
    0x7c, 0x6a, 0xd7, 0xcb, 0xc8, 0x23, 0x78, 0x30, 0xe7, 0x73, 0x76, 0x63, 0x4b, 0x37, 0x31, 0x62,
    0x2e, 0xaf, 0x30, 0xd9, 0x2e, 0x22, 0xa3, 0x88, 0x6f, 0xf1, 0x09, 0x27, 0x9d, 0x98, 0x30, 0xda,
    0xc7, 0x27, 0xaf, 0xb9, 0x4a, 0x83, 0xee, 0x6d, 0x83, 0x60, 0xcb, 0xdf, 0xa2, 0xcc, 0x06, 0x40 };
  return evaluate_result(2,
    scrypt("password", 8, "NaCl", 4, 1024, 8, 16, res, 64),
    exp, sizeof(exp), res, sizeof(res));
}

char test_3 () {
  uint8_t res[64];
  int status;
  uint8_t exp[] = {
    0x70, 0x23, 0xbd, 0xcb, 0x3a, 0xfd, 0x73, 0x48, 0x46, 0x1c, 0x06, 0xcd, 0x81, 0xfd, 0x38, 0xeb,
    0xfd, 0xa8, 0xfb, 0xba, 0x90, 0x4f, 0x8e, 0x3e, 0xa9, 0xb5, 0x43, 0xf6, 0x54, 0x5d, 0xa1, 0xf2,
    0xd5, 0x43, 0x29, 0x55, 0x61, 0x3f, 0x0f, 0xcf, 0x62, 0xd4, 0x97, 0x05, 0x24, 0x2a, 0x9a, 0xf9,
    0xe6, 0x1e, 0x85, 0xdc, 0x0d, 0x65, 0x1e, 0x40, 0xdf, 0xcf, 0x01, 0x7b, 0x45, 0x57, 0x58, 0x87 };
  return evaluate_result(3,
    scrypt("pleaseletmein", 13, "SodiumChloride", 14, 16384, 8, 1, res, sizeof(res)),
    exp, sizeof(exp), res, sizeof(res));
}

char test_4 () {
  uint8_t res[64];
  int status;
  uint8_t exp[] = {
    0x21, 0x01, 0xcb, 0x9b, 0x6a, 0x51, 0x1a, 0xae, 0xad, 0xdb, 0xbe, 0x09, 0xcf, 0x70, 0xf8, 0x81,
    0xec, 0x56, 0x8d, 0x57, 0x4a, 0x2f, 0xfd, 0x4d, 0xab, 0xe5, 0xee, 0x98, 0x20, 0xad, 0xaa, 0x47,
    0x8e, 0x56, 0xfd, 0x8f, 0x4b, 0xa5, 0xd0, 0x9f, 0xfa, 0x1c, 0x6d, 0x92, 0x7c, 0x40, 0xf4, 0xc3,
    0x37, 0x30, 0x40, 0x49, 0xe8, 0xa9, 0x52, 0xfb, 0xcb, 0xf4, 0x5c, 0x6f, 0xa7, 0x7a, 0x41, 0xa4 };
  return evaluate_result(4,
    scrypt("pleaseletmein", 13, "SodiumChloride", 14, 1048576, 8, 1, res, sizeof(res)),
    exp, sizeof(exp), res, sizeof(res));
}

void main () {
/*
  if (test_1() && test_2() && test_3()) {
  printf("%s\n", "success - all tests passed.");
  }
*/
  uint8_t* str;
  size_t str_len = 0;
  int status;
  status = scrypt_to_string("", 0, 0, 0, 0, 0, 0, 0, &str, &str_len);
  //display_byte_array(str, str_len);
  printf("%s\n", str);
}
