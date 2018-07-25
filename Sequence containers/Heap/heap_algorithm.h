#pragma once

//�����βα���vectorͷβ����ǰԪ������Ϊvectorβ��Ԫ��
template <class RandomAccessIterator>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
	__push_heap_aux(first, last, distance_type(first),value_type(first));
}

template <class RandomAccessIterator, class Distance, class T>
inline void __push_heap_aux(RandomAccessIterator first,RandomAccessIterator last, Distance*, T*) {
	//������β�˼�Ϊ��һ������(last-first)-1
	__push_heap(first, Distance((last - first) - 1), Distance(0),T(*(last - 1)));
}

template <class RandomAccessIterator, class Distance, class T>
void __push_heap(RandomAccessIterator first, Distance holeIndex,Distance topIndex, T value) {
	Distance parent = (holeIndex - 1) / 2;//�ҳ����ڵ�
	while (holeIndex > topIndex && *(first + parent) < value) {//��δ���ﶥ������ֵ���ڸ��ڵ�
		*(first + holeIndex) = *(first + parent);//�ֵΪ��ֵ
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;//���¸��ڵ�
	}
	//����ѭ��ʱ���¶�ֵ
	*(first + holeIndex) = value;
}


//��heap�ĸ��ڵ㣨���ֵ��ȡ�ߣ���ʵ������vectorβ����
template <class RandomAccessIterator>
inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last) {
	__pop_heap_aux(first, last, value_type(first));
}

template <class RandomAccessIterator, class T>
inline void __pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last, T*) {
	//��[first,last-1)��ִ��__pop_heap
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
inline void __pop_heap(RandomAccessIterator first,RandomAccessIterator last,RandomAccessIterator result,T value, Distance*) {
	*result = *first;//��β������Ϊȡ��ֵ,��ԭ����β��ֵ��Ϊ��value
	__adjust_heap(first, Distance(0), Distance(last - first), value);
}

template <class RandomAccessIterator, class Distance, class T>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex,Distance len, T value) {
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * holeIndex + 2;//��������
	while (secondChild < len) {//��ǰ�д�������
		//�Դ�secondChild����ϴ��ӽڵ�
		if (*(first + secondChild) < *(first + (secondChild - 1)))//����С������
			secondChild--;
		*(first + holeIndex) = *(first + secondChild);//�����㸳ֵΪ�ϴ���ֵ
		holeIndex = secondChild;//���¶���
		secondChild = 2 * (holeIndex + 1);//�ٴ�ָ�򶴵�����
	}
	if (secondChild == len) {//���������ӣ���������ֵΪ��ֵ���������Ŵ��������ӣ����ӱ�Ϊ���׽ڵ�������ߣ�
		*(first + holeIndex) = *(first + (secondChild - 1));
		holeIndex = secondChild - 1;
	}
	//��ԭ�е�ĩ��Ԫ�����루�����Ǹ���
	__push_heap(first, holeIndex, topIndex, value);
}

//�����򣨲��ϵ���pop_heap���ɣ�
template <class RandomAccessIterator>
void sort_heap(RandomAccessIterator first,RandomAccessIterator last) {
	while (first!=last)
		pop_heap(first, last--);
}

template <class RandomAccessIterator>
inline void make_heap(RandomAccessIterator first,RandomAccessIterator last) {
	__make_heap(first, last, value_type(first), distance_type(first));
}

template <class RandomAccessIterator, class T, class Distance>
void __make_heap(RandomAccessIterator first,RandomAccessIterator last, T*,Distance*) {
	//�����������ʸ�Ϊroot�ĵ�
	if (last - first < 2) return;
	Distance len = last - first;
	//�ҵ���Ҫ���ŵ�ͷ������Ϊ���һ�����ʸ�Ϊroot�ĵ㣩
	Distance holeIndex = (len - 2) / 2;
	while (true) {
		//������holeIndexΪ�׵�����,len�Է�Խ��
		__adjust_heap(first, holeIndex, len, T(*(first + holeIndex)));
		if (holeIndex == 0) return;
		holeIndex--;
	}
}