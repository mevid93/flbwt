#include <gtest/gtest.h>
#include "packed_array.hpp"

TEST(packed_array_test, packed_array_1)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 100, false);
    EXPECT_EQ(10U, pa->get_length());
    EXPECT_EQ(false, pa->supports_negative_integers());
    EXPECT_EQ(7U, pa->get_integer_bits());
    EXPECT_TRUE(pa->get_raw_signs_pointer() == NULL);
    EXPECT_TRUE(pa->get_raw_arr_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, packed_array_2)
{

    flbwt::PackedArray *pa = new flbwt::PackedArray(100, true, 64);
    EXPECT_EQ(100U, pa->get_length());
    EXPECT_EQ(true, pa->supports_negative_integers());
    EXPECT_EQ(64U, pa->get_integer_bits());
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    EXPECT_TRUE(pa->get_raw_arr_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_8bit_1)
{
    // pack 8-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xff, false);
    pa->set_value(0, 1);   // 0x01
    pa->set_value(1, 10);  // 0x0a
    pa->set_value(2, 100); // 0x64
    pa->set_value(7, 28);  // 0x1c
    pa->set_value(9, 255); // 0xff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x010A64000000001CU, arr[0] & 0xFFFFFF00000000FF);
    EXPECT_EQ(0x00ff000000000000U, arr[1] & 0x00FF000000000000);
    delete pa;
}

TEST(packed_array_test, set_value_8bit_2)
{
    // pack signed integers that can stored to 8 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(65, 0xff, true); // abs(-255) = 0xff
    pa->set_value(0, -1);                                            // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                           // 0x0a (excluding sign bit)
    pa->set_value(2, -100);                                          // 0x64 (excluding sign bit)
    pa->set_value(7, -28);                                           // 0x1c (excluding sign bit)
    pa->set_value(9, -255);                                          // 0xff (excluding sign bit)
    pa->set_value(64, -1);
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x010A64000000001CU, arr[0] & 0xFFFFFF00000000FF);
    EXPECT_EQ(0x00ff000000000000U, arr[1] & 0x00FF000000000000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_16bit_1)
{
    // pack 16-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffff, false);
    pa->set_value(0, 1);     // 0x01
    pa->set_value(1, 10);    // 0x0a
    pa->set_value(2, 100);   // 0x64
    pa->set_value(3, 28);    // 0x1c
    pa->set_value(4, 65535); // 0xffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0001000A0064001CU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xffff000000000000U, arr[1] & 0xFFFF000000000000);
    delete pa;
}

TEST(packed_array_test, set_value_16bit_2)
{
    // pack signed integers that can stored to 16 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffff, true); // abs(-65535) = 0xffff
    pa->set_value(0, -1);                                              // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                             // 0x0a (excluding sign bit)
    pa->set_value(2, -100);                                            // 0x64 (excluding sign bit)
    pa->set_value(3, -28);                                             // 0x1c (excluding sign bit)
    pa->set_value(4, -65535);                                          // 0xffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0001000A0064001CU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xffff000000000000U, arr[1] & 0xFFFF000000000000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_24bit_1)
{
    // pack 24-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffff, false);
    pa->set_value(0, 1);        // 0x01
    pa->set_value(1, 10);       // 0x0a
    pa->set_value(2, 15728638); // 0xeffffe
    pa->set_value(3, 28);       // 0x1c
    pa->set_value(9, 16777215); // 0xffffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x00000100000AEFFFU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xFE00001C00000000U, arr[1] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x000000FFFFFF0000U, arr[3] & 0x000000FFFFFF0000);
    delete pa;
}

TEST(packed_array_test, set_value_24bit_2)
{
    // pack signed integers that can stored to 24 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffff, true); // abs(-16777215) = 0xffffff
    pa->set_value(0, -1);                                                // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                               // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                         // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                               // 0x1c (excluding sign bit)
    pa->set_value(9, -16777215);                                         // 0xffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x00000100000AEFFFU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xFE00001C00000000U, arr[1] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x000000FFFFFF0000U, arr[3] & 0x000000FFFFFF0000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_32bit_1)
{
    // pack 32-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffff, false);
    pa->set_value(0, 1);          // 0x01
    pa->set_value(1, 10);         // 0x0a
    pa->set_value(2, 15728638);   // 0xeffffe
    pa->set_value(3, 28);         // 0x1c
    pa->set_value(9, 4294967295); // 0xffffffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x000000010000000AU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00EFFFFE0000001CU, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00000000FFFFFFFFU, arr[4] & 0x00000000FFFFFFFF);
    delete pa;
}

TEST(packed_array_test, set_value_32bit_2)
{
    // pack signed integers that can stored to 32 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffff, true); // abs(-4294967295) = 0xffffffff
    pa->set_value(0, -1);                                                  // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                                 // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                           // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                                 // 0x1c (excluding sign bit)
    pa->set_value(9, -4294967295);                                         // 0xffffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x000000010000000AU, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00EFFFFE0000001CU, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00000000FFFFFFFFU, arr[4] & 0x00000000FFFFFFFF);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_40bit_1)
{
    // pack 40-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffff, false);
    pa->set_value(0, 1);             // 0x01
    pa->set_value(1, 10);            // 0x0a
    pa->set_value(2, 15728638);      // 0xeffffe
    pa->set_value(3, 28);            // 0x1c
    pa->set_value(9, 1099511627775); // 0xffffffffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000001000000U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x000A0000EFFFFE00U, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000001C00000000U, arr[2] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x0000000000FFFFFFU, arr[5] & 0x0000000000FFFFFF);
    EXPECT_EQ(0xFFFF000000000000U, arr[6] & 0xFFFF000000000000);
    delete pa;
}

TEST(packed_array_test, set_value_40bit_2)
{
    // pack signed integers that can stored to 40 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffff, true); // abs(-1099511627775) = 0xffffffffff
    pa->set_value(0, -1);                                                    // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                                   // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                             // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                                   // 0x1c (excluding sign bit)
    pa->set_value(9, -1099511627775);                                        // 0xffffffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000001000000U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x000A0000EFFFFE00U, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000001C00000000U, arr[2] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x0000000000FFFFFFU, arr[5] & 0x0000000000FFFFFF);
    EXPECT_EQ(0xFFFF000000000000U, arr[6] & 0xFFFF000000000000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_48bit_1)
{
    // pack 48-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffff, false);
    pa->set_value(0, 1);               // 0x01
    pa->set_value(1, 10);              // 0x0a
    pa->set_value(2, 15728638);        // 0xeffffe
    pa->set_value(3, 28);              // 0x1c
    pa->set_value(9, 281474976710655); // 0xffffffffffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000010000U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000000A000000EFU, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xFFFE00000000001CU, arr[2] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x000000000000FFFFU, arr[6] & 0x000000000000FFFF);
    EXPECT_EQ(0xFFFFFFFF00000000U, arr[7] & 0xFFFFFFFF00000000);
    delete pa;
}

TEST(packed_array_test, set_value_48bit_2)
{
    // pack signed integers that can stored to 48 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffff, true); // abs(-281474976710655) = 0xffffffffffff
    pa->set_value(0, -1);                                                      // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                                     // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                               // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                                     // 0x1c (excluding sign bit)
    pa->set_value(9, -281474976710655);                                        // 0xffffffffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000010000U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000000A000000EFU, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0xFFFE00000000001CU, arr[2] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x000000000000FFFFU, arr[6] & 0x000000000000FFFF);
    EXPECT_EQ(0xFFFFFFFF00000000U, arr[7] & 0xFFFFFFFF00000000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_56bit_1)
{
    // pack 56-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffff, false);
    pa->set_value(0, 1);                 // 0x01
    pa->set_value(1, 10);                // 0x0a
    pa->set_value(2, 15728638);          // 0xeffffe
    pa->set_value(3, 28);                // 0x1c
    pa->set_value(9, 72057594037927935); // 0xffffffffffffff
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000000100U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00000000000A0000U, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000EFFFFE000000U, arr[2] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000001C00000000U, arr[3] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x00000000000000FFU, arr[7] & 0x00000000000000FF);
    EXPECT_EQ(0xFFFFFFFFFFFF0000U, arr[8] & 0xFFFFFFFFFFFF0000);
    delete pa;
}

TEST(packed_array_test, set_value_56bit_2)
{
    // pack signed integers that can stored to 56 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffff, true); // abs(-72057594037927935) = 0xffffffffffffff
    pa->set_value(0, -1);                                                        // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                                       // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                                 // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                                       // 0x1c (excluding sign bit)
    pa->set_value(9, -72057594037927935);                                        // 0xffffffffffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000000100U, arr[0] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x00000000000A0000U, arr[1] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000EFFFFE000000U, arr[2] & 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(0x0000001C00000000U, arr[3] & 0xFFFFFFFF00000000);
    EXPECT_EQ(0x00000000000000FFU, arr[7] & 0x00000000000000FF);
    EXPECT_EQ(0xFFFFFFFFFFFF0000U, arr[8] & 0xFFFFFFFFFFFF0000);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, set_value_64bit_1)
{
    // pack 56-bit unsigned integers
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffffff, false);
    pa->set_value(0, 1);                   // 0x01
    pa->set_value(1, 10);                  // 0x0a
    pa->set_value(2, 15728638);            // 0xeffffe
    pa->set_value(3, 28);                  // 0x1c
    pa->set_value(9, 9223372036854775807); // 7FFFFFFFFFFFFFFF (max signed)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000000001U, arr[0]);
    EXPECT_EQ(0x000000000000000AU, arr[1]);
    EXPECT_EQ(0x0000000000EFFFFEU, arr[2]);
    EXPECT_EQ(0x000000000000001CU, arr[3]);
    EXPECT_EQ(0x7FFFFFFFFFFFFFFFU, arr[9]);
    delete pa;
}

TEST(packed_array_test, set_value_64bit_2)
{
    // pack signed integers that can stored to 56 bits without sign bit
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffffff, true); // abs(-18446744073709551615) = 0xffffffffffffffff
    pa->set_value(0, -1);                                                          // 0x01 (excluding sign bit)
    pa->set_value(1, -10);                                                         // 0x0a (excluding sign bit)
    pa->set_value(2, -15728638);                                                   // 0xeffffe (excluding sign bit)
    pa->set_value(3, -28);                                                         // 0x1c (excluding sign bit)
    pa->set_value(9, -9223372036854775807);                                        // 0x7fffffffffffffff (excluding sign bit)
    uint64_t *arr = pa->get_raw_arr_pointer();
    EXPECT_EQ(0x0000000000000001U, arr[0]);
    EXPECT_EQ(0x000000000000000AU, arr[1]);
    EXPECT_EQ(0x0000000000EFFFFEU, arr[2]);
    EXPECT_EQ(0x000000000000001CU, arr[3]);
    EXPECT_EQ(0x7FFFFFFFFFFFFFFFU, arr[9]);
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, get_value_8bit_1)
{
    // pack and retrieve signed integers (8-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xff, true);
    pa->set_value(0, 1);   
    pa->set_value(1, -10); 
    pa->set_value(2, 100); 
    pa->set_value(7, -28); 
    pa->set_value(9, 255); 
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(100, pa->get_value(2));
    EXPECT_EQ(-28, pa->get_value(7));
    EXPECT_EQ(0, pa->get_value(8));
    EXPECT_EQ(255, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_16bit_1)
{
    // pack and retrieve signed integers (16-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffff, true); 
    pa->set_value(0, 1);                                               
    pa->set_value(1, -10);                                             
    pa->set_value(2, 100);                                            
    pa->set_value(3, -28);                                           
    pa->set_value(5, -65535);                                        
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(100, pa->get_value(2));
    EXPECT_EQ(-28, pa->get_value(3));
    EXPECT_EQ(-65535, pa->get_value(5));
    delete pa;
}

TEST(packed_array_test, get_value_24bit_1)
{
    // pack and retrieve signed integers (24-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffff, true);
    pa->set_value(0, 1);                                               
    pa->set_value(1, -10);                                              
    pa->set_value(2, 15728638);                                        
    pa->set_value(3, -28);                                              
    pa->set_value(9, -16777215);                
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(15728638, pa->get_value(2));
    EXPECT_EQ(-28, pa->get_value(3));
    EXPECT_EQ(-16777215, pa->get_value(9));
    pa->set_value(9, 0); 
    EXPECT_EQ(0, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_32bit_1)
{
    // pack and retrieve signed integers (32-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffff, true);
    pa->set_value(0, -1);                                                 
    pa->set_value(1, 10);                                                
    pa->set_value(2, -15728638);                                          
    pa->set_value(3, 28);                                                
    pa->set_value(9, 4294967295);
    EXPECT_EQ(-1, pa->get_value(0));
    EXPECT_EQ(10, pa->get_value(1));
    EXPECT_EQ(-15728638, pa->get_value(2));
    EXPECT_EQ(28, pa->get_value(3));
    EXPECT_EQ(4294967295, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_40bit_1)
{
    // pack and retrieve signed integers (40-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffff, true);
    pa->set_value(0, 1);                                    
    pa->set_value(1, -10);                                  
    pa->set_value(2, 15728638);                         
    pa->set_value(3, 28);                
    pa->set_value(9, -1099511627775);          
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(15728638, pa->get_value(2));
    EXPECT_EQ(28, pa->get_value(3));
    EXPECT_EQ(-1099511627775, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_48bit_1)
{
    // pack and retrieve signed integers (48-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffff, true);
    pa->set_value(0, -1);               
    pa->set_value(1, 10);          
    pa->set_value(2, -15728638);       
    pa->set_value(3, 28);             
    pa->set_value(9, 281474976710655);       
    EXPECT_EQ(-1, pa->get_value(0));
    EXPECT_EQ(10, pa->get_value(1));
    EXPECT_EQ(-15728638, pa->get_value(2));
    EXPECT_EQ(28, pa->get_value(3));
    EXPECT_EQ(281474976710655, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_56bit_1)
{
    // pack and retrieve signed integers (56-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffff, true);
    pa->set_value(0, 1);              
    pa->set_value(1, -10);            
    pa->set_value(2, 15728638);      
    pa->set_value(3, 28);          
    pa->set_value(9, -72057594037927935); 
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(15728638, pa->get_value(2));
    EXPECT_EQ(28, pa->get_value(3));
    EXPECT_EQ(-72057594037927935, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, get_value_64bit_1)
{
    // pack and retrieve signed integers (64-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 0xffffffffffffffff, true);
    pa->set_value(0, 1);                           
    pa->set_value(1, -10);                                    
    pa->set_value(2, 15728638);                       
    pa->set_value(3, -28);                       
    pa->set_value(9, -9223372036854775807);  
    EXPECT_EQ(1, pa->get_value(0));
    EXPECT_EQ(-10, pa->get_value(1));
    EXPECT_EQ(15728638, pa->get_value(2));
    EXPECT_EQ(-28, pa->get_value(3));
    EXPECT_EQ(-9223372036854775807, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, extra_test_1)
{
    // pack and retrieve signed integers (24-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(30000, 0xFFFFFF, false);
    pa->set_value(29098, 1);
    EXPECT_EQ(1, pa->get_value(29098));
    pa->set_value(29098, 2);
    EXPECT_EQ(2, pa->get_value(29098));
    delete pa;
}