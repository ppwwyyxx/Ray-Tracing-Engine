// File: matrix.hh
// Date: Tue Apr 09 13:47:21 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "debugutils.hh"

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
				if (v)
					memcpy(val[i], v[i], rowlen);
			}
		}

		~Matrix() {
			for (int i = 0; i < h; i++)
				delete [] val[i];
			delete [] val;
		}

		Matrix(const Matrix&) = delete;

		Matrix & operator = (const Matrix &) = delete;

		// get the ith row
		T*& operator [] (int i) {
			m_assert((0 <= i) && (i <= h));
			return val[i];
		}

		/*
		 *const T*& operator [] (int i) const {
		 *    m_assert((0 <= i) && (i <= h));
		 *    return val[i];
		 *}
		 */
};

