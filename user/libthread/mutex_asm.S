.global atomic_add_and_update

atomic_add_and_update:
	movl    0x4(%esp), %ecx
	movl    0x8(%esp), %eax
	lock xadd %eax, (%ecx)
	ret  

