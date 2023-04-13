import sys
import csv

csv_file_name = sys.argv[1] # Name of the file to parse
column_table_index = sys.argv[2] # The column which should be used as an index
column_table_index_base = int(sys.argv[3]) # The base of that column (i.e. base 16, base 10, base 2) expressed in base 10
column_table_value = sys.argv[4] # The column which should be used as a value
delimiter_ascii = str(chr(int(sys.argv[5]))) # The delimiter between each column
table_entry_count = int(sys.argv[6]) # Base 10 length of the array aligned to the nearest power of 2
value_data_type = str(sys.argv[7]) # The type of the array "int", "string", etc...

with open(csv_file_name, newline='') as csv_file:
        spam_reader = csv.reader(csv_file, delimiter=delimiter_ascii)

        idx_col_index = 0
        idx_col_value = 0

        for row in spam_reader:
                idx_col_index = row.index(column_table_index)
                idx_col_value = row.index(column_table_value)
                break

        row_count = 0

        a_table = [0] * table_entry_count

        print(column_table_index,":",column_table_value)
        for row in spam_reader:
                a_table[int(row[idx_col_index], base=column_table_index_base)] = row[idx_col_value]
                
                print(row[idx_col_index],":",row[idx_col_value])
                row_count += 1

        print(row_count,"rows")

        if (value_data_type == "int"):
                print("int array[] = { ")

                array_str = ""
                for i in range(table_entry_count):
                        array_str += str(a_table[i])
                        array_str += ','
                        if (i != table_entry_count - 1):
                               array_str += ' '

                print(array_str)
                print("};")

                        
                
        