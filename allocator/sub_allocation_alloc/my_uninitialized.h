#pragma once
#include"my_construct.h"

template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
	return __uninitialized_copy(first, last, result, value_type(first));
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator  __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T *) {
	using is_POD =typename  __type_traits<T>::is_POD_type;
	return __uninitialized_copy_aux(first, last, result, is_POD());
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator  __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
	return copy(first, last, result);
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator  __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
	ForwardIterator cur = result;
	for (; first != last; ++cur,++first) {
		construct(&*cur, *first);
	}
	return cur;
}

//���char*��wchar_t*�����ػ��汾 memmoveֱ���ƶ��ڴ�
inline char*  uninitialized_copy(char* first, char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t*  uninitialized_copy(wchar_t* first, wchar_t* last, wchar_t* result) {
	memmove(result, first, sizeof(wchar_t)*(last - first));
	return result + (last - first);
}

template <class ForwardIterator, class T>
inline ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
	return __uninitialized_fill(first, last, value, value_type(first));
}

template<class ForwardIterator, class T, class T1>
inline ForwardIterator  __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value, T1*) {
	using is_POD = typename __type_traits<T1>::is_POD_type;
	return __uninitialized_fill_aux(first, last, value, is_POD());
}

template<class ForwardIterator, class T>
inline ForwardIterator  __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& value, __true_type) {
	return fill(first, last, value);
}

template<class ForwardIterator, class T>
inline ForwardIterator  __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& value, __false_type) {
	ForwardIterator cur = first;
	for (:cur != last; ++cur) {
		construct(&*cur, value);
	}
	return cur;
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T & x) {
	//����value_typeȡ��first��value_type(���������һ�£�
	return __uninitialized_fill_n(first, n, x, value_type(first));
}

template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator  __uninitialized_fill_n(ForwardIterator first, Size n, const T & x, T1*) {
	//POD means Plain Old Data
	//��Ȼӵ��trivial ctor/dtot/copy/assignment����
	using is_POD = typename __type_traits<T1>::is_POD_type;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator  __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T & x,__true_type) {
	//POD���Ϳ�ֱ�Ӳ��ó�ֵ��дʽ�ַ�
	return fill(first, n, x);//ת���߽׺���ִ��
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator  __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T & x, __false_type) {
	//�����쳣����
	//��ȷ��ȷ����һ��һ��������ʧ������Ҫ�������ж���
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur) {
		construct(&*cur, x);
	}
	return cur;
}

