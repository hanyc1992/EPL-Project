Valarray_1.h:	ֻ���vector<double>�ļӷ���������û����

Valarray_2.h:	������Binary operation���Ҷ�int/double����û����

Valarray_3.h:	��2�����ϼ�����Unary operation,Ҳ���Ǹ���

Valarray_4.h:	������sum��accumulate

Valarray_5.h:	�ּ�����apply�����Ƕ�apply��Щ�죬��Ҫ����ϸ����

Valarray_6.h:	������complex��type_choose


Valarray_7.h: ��apply������Ϊoperator[]�� ��BinaryOp��UnaryOp�м�����[]��=������ͨ��test0_1��д��Scalar��struct,����is_buv��enable_if��дoperator+-*/��Ŀǰ�Ѿ�д�˼ӷ���

				ע������û�м�is_ref��ֱ��copy������
				ע��sum��accumulate�����⣬Ӧ����Ҫ��BI��UN��Ҳд
				����Ҫ����is_ref����ʦ�Ͽν��ģ�
				
Valarray_8.h: ��7��д�����е�+-*/ overload

Valarray_9.h:	��д��AddOp��Щ��������������complex<double> + float��ֵ��ע����ǣ�complex�ĺ�������û�н�intת��Ϊcomplex<double>�Ĺ��캯�������Զ�AddOp��Щ�Ĳ���ȫ��д�ˣ�������������complex<double>+int�����
				�Ҵ�ʱ�Ѿ�ͨ��PhaseA������3�������ļ�

Valarray_10.h	������is_ref

Valarray_11.h	��Binary��Unary��class�м�����iterator
	
Valarray_12.h	��Binary��Unary����valarray��class�м�����sum��accumulate

Valarray_13.h	������operator<< for ostream

Valarray_14.h	��Binary��Unary�н�operator[]��д��������const�ͷ�const��
				��AddOp��Щ�е�apply������д��operator()������Ŀ����Ϊ���ܺ�apply/ sqrt�ĺ���ָ������ƥ��
				�Ұ�Valarray_9.h���ֶ�д������complex<double> + float�Ļ�ȥ�ˣ���������type_cast������֮ǰ���constructor���������Թ���----��������complex<int> + double�Ļ������⣬��ΪSTL��û�д�complex<int> ��complex<double>��type_cast�����Ǻ��񲻻������
				
Valarray_15.h	���հ�
				��Binary��Unary�н�operator[]��д��1��-----��Ϊ���ǵ����������м�ı���Binary��Unary�е�ֵ
				��Binary��Unary��valarray��д��apply��sqrt���������ﻹ��ֻ�����function object---��Ϊ��ʵ���ֲ���˵������result_type???
						ע�⵽����������Unary�����У�value_type����op��t��ͬ�����ģ�eg. sqrt����һ��Unary��t����������int����value_type����double.�����������OP::result_type
		

Valarray_16.h	�����հ�
				��valarray��class������д��assignment operator(3����valarray, Binary, Unary),����
							��When a valarray is assigned another valarray, its resulting size should be the minimum of its current size and the size of the other valarray. (I had previously stated incorrectly that it should always take the size of the valarray on the right-hand side.)�� 
				assignment operator���Ѽ������������һ����(��GCC��ͨ�����������ָĳ�3����)
				
				Ҳ����copy constructor
				
Valarray_17.h	������
				��valarray�м���3��assignment operator��Ӧ����valarray<double> = double/ valarray<double> = valarray<int>�����
				�Լ���accumulate��sum�ĺ����ķ������͸ĳ�ΪFUNC::result_type
				
Valarray_18.h	����������
				��17�Ļ����϶Գ����������΢�ļ򻯣�����һЩע�ͣ���PhaseC���Ͻ��汾
		
Valarray_19.h	������������
				��18�Ļ����Ͻ��о�����Ҫ�Ķ���iteratorд����һ��template class
		
VS�汾ͬ19		







���� operator+-*/ ���� ����complex<int> + double�Ĳ�����Ҫ����argument��ѡ��ͬ�ĺ����������õ�SFINAE������enable_if��������ע�⵽enable_if��member function��ʹ��ʱҪע���Ƿ��в���deduction���������2�����ӽ��͵Ĳ���
http://stackoverflow.com/questions/6627651/enable-if-method-specialization/6627748#6627748
http://stackoverflow.com/questions/13401716/selecting-a-member-function-using-different-enable-if-conditions
		���ʵ�ֵ�ʱ��operator+-*/ʹ����enable_if����Ϊeg. operator+���ݲ�ͬ��argument�ķ���ֵ��ͬ����complex<int> + double����������1�еĵ�һ����������Ϊ����ֵ����ͬ�����ú���ѡ��ķ�������ȻҲ������enable_if��