Valarray_1.h:	只针对vector<double>的加法可以做了没问题

Valarray_2.h:	对所有Binary operation，且对int/double操作没问题

Valarray_3.h:	在2基础上加上了Unary operation,也就是负号

Valarray_4.h:	加上了sum和accumulate

Valarray_5.h:	又加上了apply。但是对apply有些混，需要再仔细看看

Valarray_6.h:	加上了complex的type_choose


Valarray_7.h: 将apply函数改为operator[]； 在BinaryOp和UnaryOp中加入了[]和=，可以通过test0_1；写了Scalar的struct,利用is_buv和enable_if来写operator+-*/。目前已经写了加法的

				注意现在没有加is_ref，直接copy了数据
				注意sum和accumulate有问题，应该是要在BI和UN里也写
				还需要加上is_ref（老师上课讲的）
				
Valarray_8.h: 在7上写了所有的+-*/ overload

Valarray_9.h:	改写了AddOp这些，用来处理诸如complex<double> + float。值得注意的是，complex的函数库中没有将int转变为complex<double>的构造函数，所以对AddOp这些的操作全手写了，用来处理例如complex<double>+int的情况
				且此时已经通过PhaseA的所有3个测试文件

Valarray_10.h	加上了is_ref

Valarray_11.h	在Binary和Unary的class中加入了iterator
	
Valarray_12.h	在Binary和Unary还有valarray的class中加入了sum和accumulate

Valarray_13.h	加入了operator<< for ostream

Valarray_14.h	在Binary和Unary中将operator[]补写成两个（const和非const）
				把AddOp这些中的apply函数改写成operator()函数，目的是为了能和apply/ sqrt的函数指针类型匹配
				且把Valarray_9.h中手动写的诸如complex<double> + float改回去了，但是用了type_cast而不是之前想的constructor，这样可以工作----对于类似complex<int> + double的会有问题，因为STL中没有从complex<int> 到complex<double>的type_cast，但是好像不会查这种
				
Valarray_15.h	最终版
				在Binary和Unary中将operator[]又写成1个-----因为考虑到并不会在中间改变在Binary和Unary中的值
				在Binary和Unary和valarray中写了apply和sqrt函数。这里还是只针对了function object---因为在实验手册上说可以用result_type???
						注意到，尤其是在Unary的类中，value_type是由op和t共同决定的，eg. sqrt返回一个Unary，t的类型若是int，则value_type还是double.这里就利用了OP::result_type
		

Valarray_16.h	最最终版
				在valarray的class里重新写了assignment operator(3个，valarray, Binary, Unary),满足
							“When a valarray is assigned another valarray, its resulting size should be the minimum of its current size and the size of the other valarray. (I had previously stated incorrectly that it should always take the size of the valarray on the right-hand side.)” 
				assignment operator最后把几个合起来变成一个了(在GCC上通不过，所以又改成3个了)
				
				也加了copy constructor
				
Valarray_17.h	究极版
				在valarray中加了3个assignment operator，应对如valarray<double> = double/ valarray<double> = valarray<int>的情况
				以及吧accumulate和sum的函数的返回类型改成为FUNC::result_type
				
Valarray_18.h	究级进化版
				在17的基础上对程序进行了略微的简化，加上一些注释，是PhaseC的上交版本
		
Valarray_19.h	究究级进化版
				在18的基础上进行精简，主要改动有iterator写成了一个template class
		
VS版本同19		







关于 operator+-*/ 还有 例如complex<int> + double的操作需要根据argument来选择不同的函数，可以用到SFINAE（如用enable_if）。但是注意到enable_if在member function中使用时要注意是否有参数deduction。下面这个2个链接解释的不错
http://stackoverflow.com/questions/6627651/enable-if-method-specialization/6627748#6627748
http://stackoverflow.com/questions/13401716/selecting-a-member-function-using-different-enable-if-conditions
		最后实现的时候，operator+-*/使用了enable_if，因为eg. operator+根据不同的argument的返回值不同；在complex<int> + double中用了链接1中的第一个方法，因为返回值都相同可以用函数选择的方法（当然也可以用enable_if）