/**
 * The MIT License (MIT)
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to 
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VCFDIFF_H
#define VCFDIFF_H

#ifdef HTSDIR

#include "headers.h"
#include "lut.h"
#include "vcf.h"
#include "htslib/synced_bcf_reader.h"
#include "known_field_info.h"

//Exceptions thrown
class VCFDiffException : public std::exception{
  public:
    VCFDiffException(const std::string m="") : msg_("VCFDiffException : "+m) { ; }
    ~VCFDiffException() { ; }
    // ACCESSORS
    /** Returns the exception message. */
    const char* what() const noexcept { return msg_.c_str(); }
  private:
    std::string msg_;
};


class VCFDiffFile
{
  public:
    VCFDiffFile(const std::string& filename, const std::string& regions="");
    ~VCFDiffFile();
    void setup_lut(const std::set<std::string>& gold_set, const std::set<std::string>& test_set,
        const int bcf_dt_type, GoldLUT& lut, const VCFDiffFile& gold);
    void setup_luts(const VCFDiffFile& gold, const bool use_callsets_file_for_samples);
    void set_regions_and_open_file();
    void seek_and_read(const int rid, const int pos);
    void read_and_advance();
    void reset_field_to_line_idx_mapping()
    {
      //-1
      memset(&(m_fields_in_gold_line[0]), -1, m_fields_in_gold_line.size()*sizeof(int));
      memset(&(m_fields_in_test_line[0]), -1, m_fields_in_test_line.size()*sizeof(int));
    }
    void print_line(std::ostream& fptr=std::cerr);
    void compare_line(const bcf_hdr_t* gold_hdr, bcf1_t* gold_line);
    bool compare_unequal_fields(const bcf_hdr_t* gold_hdr, const bcf1_t* gold_line, const int bcf_field_type, std::string& error_message);
    template<class T1, class T2>
    bool compare_unequal_vector(const bcf_hdr_t* gold_hdr, const bcf1_t* gold_line, const int bcf_field_type,
        int gold_line_field_pos_idx, int test_line_field_pos_idx);
    std::string create_region(const std::string& regions,
        const std::unordered_map<std::string, std::pair<int64_t, int64_t>>& regions_contig_to_interval, const std::string& contig);
  public:
    std::string m_filename;
    std::string m_regions;
  private:
    bcf_srs_t* m_reader;
  public:
    int64_t m_num_lines_read;
    bcf_hdr_t* m_hdr;
    bcf1_t* m_line;
    std::set<std::string> m_samples;
    std::set<std::string> m_fields;
    std::set<std::string> m_contigs;
    GoldLUT m_samples_lut;
    GoldLUT m_fields_lut;
    GoldLUT m_contigs_lut;
    //Fields in the line
    std::vector<int> m_fields_in_gold_line;
    std::vector<int> m_fields_in_test_line;
    SchemaIdxToKnownVariantFieldsEnumLUT m_field_idx_to_known_field_enum;
    bool m_diff_alleles_flag;
    std::vector<int> m_gold_genotype_idx_to_test_idx;
    CombineAllelesLUT m_alleles_lut;
    //Temp buffer
    kstring_t m_tmp_hts_string;
};

#endif

#endif
