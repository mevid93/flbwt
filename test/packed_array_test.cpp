#include <gtest/gtest.h>
#include "packed_array.hpp"

TEST(packed_array_test, packed_array_1)
{
    flbwt::PackedArray *pa = new flbwt::PackedArray(10, 100, false);
    EXPECT_EQ(10U, pa->get_length());
    EXPECT_EQ(false, pa->supports_negative_integers());
    EXPECT_EQ(8U, pa->get_integer_bits());
    EXPECT_TRUE(pa->get_raw_signs_pointer() == NULL);
    EXPECT_TRUE(pa->get_raw_arr_pointer() != NULL);
    delete pa;
}

TEST(packed_array_test, packed_array_2)
{

    flbwt::PackedArray *pa = new flbwt::PackedArray(100, 0xffffffffffffffff, true);
    EXPECT_EQ(100U, pa->get_length());
    EXPECT_EQ(true, pa->supports_negative_integers());
    EXPECT_EQ(64U, pa->get_integer_bits());
    EXPECT_TRUE(pa->get_raw_signs_pointer() != NULL);
    EXPECT_TRUE(pa->get_raw_arr_pointer() != NULL);
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
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
    EXPECT_EQ(0, pa->get_value(4));
    EXPECT_EQ(-9223372036854775807, pa->get_value(9));
    delete pa;
}

TEST(packed_array_test, extra_test_1)
{
    // pack and retrieve signed integers (24-bits excluding sign bit)
    flbwt::PackedArray *pa = new flbwt::PackedArray(30000, 7343373, false);
    pa->set_value(29098, 1);
    EXPECT_EQ(1, pa->get_value(29098));
    pa->set_value(29098, 2);
    EXPECT_EQ(2, pa->get_value(29098));
    delete pa;
}