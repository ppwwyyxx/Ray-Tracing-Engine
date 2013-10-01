// File: matrix.hh

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include <cstring>
#include "lib/utils.hh"
#include "lib/debugutils.hh"

// basic 2-d array
template <typename T>
class Matrix {
	public:
		T **val;
		int w, h;

		Matrix(int m_w, int m_h):
			w(m_w), h(m_h) {
				val = new T* [h];
				for (int i = 0; i < h; i ++)
					val[i] = new T[w]();
		}

		Matrix(int m_w, int m_h, T** v)
			:w(m_w), h(m_h) {
			val = new T* [h];
			int rowlen = w * sizeof(T);

			for (int i = 0; i < h; i++) {
				val[i] = new T [w];
				if (v) memcpy(val[i], v[i], rowlen);
			}
		}

		~Matrix() {
			for (int i = 0; i < h; i++)
				delete [] val[i];
			delete [] val;
		}

		// something bad
		Matrix(const Matrix& m) {
			w = m.w, h = m.h;
			val = new T* [h];
			REP(i, h) {
				val[i] = new T[w]();
				memcpy(val[i], m.val[i], w * sizeof(T));
			}
		}

		Matrix & operator = (const Matrix & m) {
			if (this != &m) {
				w = m.w, h = m.h;
				val = new T* [h];
				REP(i, h) {
					val[i] = new T[w]();
					memcpy(val[i], m.val[i], w * sizeof(T));
				}
			}
			return *this;
		}

		Matrix & operator = (Matrix && r) {
			m_assert(this != &r);
			free_2d<T>(val, h);
			val = r.val;
			w = r.w, h = r.h;
			r.val = nullptr;
			return *this;
		}

		Matrix(Matrix&& r) {
			val = r.val;
			w = r.w, h = r.h;
			r.val = nullptr;
		}
		// something ugly

		// get the ith row
		T*& operator [] (int i) {
			m_assert((0 <= i) && (i <= h));
			return val[i];
		}

		const T*& operator [] (int i) const {
			m_assert((0 <= i) && (i <= h));
			return val[i];
		}

};

