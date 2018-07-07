#ifndef COMMON_FILE_READER_H
#define COMMON_FILE_READER_H

#include "lsfs.h"

#ifdef ENABLE_SD
#define IF_SD(X) X
#else
#define IF_SD(X)
#endif

#ifdef ENABLE_SERIALFLASH
#define IF_SF(X) X
#else
#define IF_SF(X)
#endif

#define IF_MEM(X) X

#define RUN_ALL(X)				\
  switch (type_) {				\
    IF_SD(case TYPE_SD: return sd_file_.X;)	\
    IF_SF(case TYPE_SF: return sf_file_.X;)	\
    IF_MEM(case TYPE_MEM: return mem_file_.X;)	\
  }

#define RUN_ALL_VOID(X)				\
  switch (type_) {				\
    IF_SD(case TYPE_SD: sd_file_.X; return;)	\
    IF_SF(case TYPE_SF: sf_file_.X; return;)	\
    IF_MEM(case TYPE_MEM: mem_file_.X; return;)	\
  }

class MemFile {
public:
  MemFile(const uint8_t* data, size_t size) :
    data_(data), size_(size), pos_(0) {
  }
  MemFile() : data_(nullptr), size_(0), pos_(0) {
  }
  int read(uint8_t* dest, size_t bytes) {
    size_t to_copy = min(available(), bytes);
    memcpy(dest, data_ + pos_, to_copy);
    pos_ += to_copy;
    return to_copy;
  }
  size_t available() {
    return size_ - pos_;
  }
  int peek() {
    if (!available()) return -1;
    return data_[pos_];
  }
  void seek(size_t pos) {
    pos_ = min(size_, pos);
  }
  size_t position() const {
    return pos_;
  }
  size_t size() const {
    return size_;
  }
  operator bool() const {
    return data_ != 0;
  }
  void close() {
    data_ = 0;
  }
private:
  const uint8_t* data_;
  size_t size_;
  size_t pos_;
};

// TODO: Make proper assignment or use std::variant instead.
class FileReader {
public:
  FileReader() : type_(TYPE_MEM) {
    mem_file_ = MemFile();
  }
  ~FileReader() { Close(); }
  bool Open(const char* filename) {
    Close();
#ifdef ENABLE_SERIALFLASH
    sf_file_ = SerialFlashChip::open(filename);
    if (sf_file_) {
      type_ = TYPE_SF;
      return true;
    }
#endif
#ifdef ENABLE_SD
    sd_file_ = LSFS::Open(filename);
    if (sd_file_) {
      type_ = TYPE_SD;
      return true;
    }
#endif    
    return false;
  }
  bool OpenMem(const uint8_t* data, uint32_t length) {
    Close();
    type_ = TYPE_MEM;
    MemFile tmp(data, length);
    mem_file_ = tmp;
    return true;
  }
  bool IsOpen() {
    switch (type_) {
      IF_SD(case TYPE_SD: return !!sd_file_;)
      IF_SF(case TYPE_SF: return !!sf_file_;)
      IF_MEM(case TYPE_MEM: return !!mem_file_;)
    }
    return false;
  };
  void Close() {
    switch (type_) {
      IF_SD(case TYPE_SD: return sd_file_.close(); sd_file_.~File();)
      IF_SF(case TYPE_SF: return sf_file_.close(); sf_file_.~SerialFlashFile();)
      IF_MEM(case TYPE_MEM: return mem_file_.close(); mem_file_.~MemFile();)
    }
    type_ = TYPE_MEM;
    mem_file_ = MemFile();
  }
  int Read(uint8_t* dest, int bytes) {
    RUN_ALL(read(dest, bytes))
    return 0;
  }
  int Read() {
    uint8_t tmp;
    if (Read(&tmp, 1)) {
      return tmp;
    } else {
      return -1;
    }
  }
  void Seek(uint32_t n) {
    RUN_ALL_VOID(seek(n))
  }
  uint32_t Available() {
    RUN_ALL(available());
    return 0;
  }
  uint32_t Tell() {
    RUN_ALL(position());
    return 0;
  }
  uint32_t FileSize() {
    RUN_ALL(size());
    return 0;
  }
  int Peek() {
    switch (type_) {
      IF_SD(case TYPE_SD: return sd_file_.peek(););
#ifdef ENABLE_SERIALFLASH
      case TYPE_SF: {
	uint8_t tmp;
	if (sf_file_.read(&tmp, 1)) {
	  sf_file_.seek(sf_file_.position() - 1);
	  return tmp;
	} else {
	  return -1;
	}
      }
#endif   
      case TYPE_MEM:
	return mem_file_.peek();
    }
    return -1;
  }
  int AlignRead(int n) {
#ifdef ENABLE_SD
    if (type_ == TYPE_SD) {
      uint32_t pos = Tell();
      uint32_t next_block = (pos + 512u) & ~511u;
      int bytes_to_end_of_block = next_block - pos;
      return min(n, bytes_to_end_of_block);
    }
#endif
    return n;
  }
  void Skip(int n) { Seek(Tell() + n); }
  void Rewind() { Seek(0); }

private:
  enum {
#ifdef ENABLE_SD
    TYPE_SD,
#endif
#ifdef ENABLE_SERIALFLASH    
    TYPE_SF,
#endif    
    TYPE_MEM
  } type_;
  union {
    IF_SD(File sd_file_;)
    IF_SF(SerialFlashFile sf_file_;)
    MemFile mem_file_;
  };
};

#endif
