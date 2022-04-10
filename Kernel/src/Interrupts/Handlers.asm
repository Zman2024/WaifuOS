[bits 64]

extern SaveSIMD
extern RestoreSIMD
extern DumpGeneralRegisters
extern GlobalInterruptTable

global GlobalHandlerStubTable

section .text

%macro ErrorHandlerStub 1
IntStub%1:
	call DumpGeneralRegisters
	pop rdi ; the error code
	call SaveSIMD
	mov rcx, %1
	call StandardHandler
	call RestoreSIMD
iretq
%endmacro

%macro HandlerStub 1
IntStub%1:
	call SaveSIMD
	mov rcx, %1
	call StandardHandler
	call RestoreSIMD
iretq
%endmacro

StandardHandler:
	shl rcx, 3
	mov rax, [GlobalInterruptTable + rcx]
	test rax, rax
	jz .ret
	jmp rax
.ret: ret





HandlerStub				0	; Divide by 0
HandlerStub				1	; Debug
HandlerStub				2	; NMI Interrupt
HandlerStub				3	; Breakpoint (INT3)
HandlerStub				4	; Overflow (INTO)
HandlerStub				5	; Bounds range exceeded (BOUND)
HandlerStub				6	; Invalid opcode (UD2)
HandlerStub				7	; Device not available (WAIT/FWAIT)
ErrorHandlerStub		8	; Double fault
HandlerStub				9	; Coprocessor segment overrun
ErrorHandlerStub		10	; Invalid TSS
ErrorHandlerStub		11	; Segment not present
ErrorHandlerStub		12	; Stack-segment fault
ErrorHandlerStub		13	; General protection fault
ErrorHandlerStub		14	; Page fault
HandlerStub				15	; Reserved
HandlerStub				16	; x87 FPU error
ErrorHandlerStub		17	; Alignment check
HandlerStub				18	; Machine check
HandlerStub				19	; SIMD Floating-Point Exception
HandlerStub				20	; PIT IRQ0
ErrorHandlerStub		21	; PIT IRQ1
HandlerStub				22	; PIT IRQ2
HandlerStub				23	; PIT IRQ3
HandlerStub				24	; PIT IRQ4
HandlerStub				25	; PIT IRQ5
HandlerStub				26	; PIT IRQ6
HandlerStub				27	; PIT IRQ7
HandlerStub				28	; PIT IRQ8
ErrorHandlerStub		29	; PIT IRQ9
ErrorHandlerStub		30	; PIT IRQ10
HandlerStub				31	; PIT IRQ11
HandlerStub				32	; PIT IRQ12
HandlerStub				33	; PIT IRQ13
HandlerStub				34	; PIT IRQ14
HandlerStub				35	; PIT IRQ15
HandlerStub				36
HandlerStub				37
HandlerStub				38
HandlerStub				39
HandlerStub				40
HandlerStub				41
HandlerStub				42
HandlerStub				43
HandlerStub				44
HandlerStub				45
HandlerStub				46
HandlerStub				47
HandlerStub				48
HandlerStub				49
HandlerStub				50
HandlerStub				51
HandlerStub				52
HandlerStub				53
HandlerStub				54
HandlerStub				55
HandlerStub				56
HandlerStub				57
HandlerStub				58
HandlerStub				59
HandlerStub				60
HandlerStub				61
HandlerStub				62
HandlerStub				63
HandlerStub				64
HandlerStub				65
HandlerStub				66
HandlerStub				67
HandlerStub				68
HandlerStub				69
HandlerStub				70
HandlerStub				71
HandlerStub				72
HandlerStub				73
HandlerStub				74
HandlerStub				75
HandlerStub				76
HandlerStub				77
HandlerStub				78
HandlerStub				79
HandlerStub				80
HandlerStub				81
HandlerStub				82
HandlerStub				83
HandlerStub				84
HandlerStub				85
HandlerStub				86
HandlerStub				87
HandlerStub				88
HandlerStub				89
HandlerStub				90
HandlerStub				91
HandlerStub				92
HandlerStub				93
HandlerStub				94
HandlerStub				95
HandlerStub				96
HandlerStub				97
HandlerStub				98
HandlerStub				99
HandlerStub				100
HandlerStub				101
HandlerStub				102
HandlerStub				103
HandlerStub				104
HandlerStub				105
HandlerStub				106
HandlerStub				107
HandlerStub				108
HandlerStub				109
HandlerStub				110
HandlerStub				111
HandlerStub				112
HandlerStub				113
HandlerStub				114
HandlerStub				115
HandlerStub				116
HandlerStub				117
HandlerStub				118
HandlerStub				119
HandlerStub				120
HandlerStub				121
HandlerStub				122
HandlerStub				123
HandlerStub				124
HandlerStub				125
HandlerStub				126
HandlerStub				127
HandlerStub				128
HandlerStub				129
HandlerStub				130
HandlerStub				131
HandlerStub				132
HandlerStub				133
HandlerStub				134
HandlerStub				135
HandlerStub				136
HandlerStub				137
HandlerStub				138
HandlerStub				139
HandlerStub				140
HandlerStub				141
HandlerStub				142
HandlerStub				143
HandlerStub				144
HandlerStub				145
HandlerStub				146
HandlerStub				147
HandlerStub				148
HandlerStub				149
HandlerStub				150
HandlerStub				151
HandlerStub				152
HandlerStub				153
HandlerStub				154
HandlerStub				155
HandlerStub				156
HandlerStub				157
HandlerStub				158
HandlerStub				159
HandlerStub				160
HandlerStub				161
HandlerStub				162
HandlerStub				163
HandlerStub				164
HandlerStub				165
HandlerStub				166
HandlerStub				167
HandlerStub				168
HandlerStub				169
HandlerStub				170
HandlerStub				171
HandlerStub				172
HandlerStub				173
HandlerStub				174
HandlerStub				175
HandlerStub				176
HandlerStub				177
HandlerStub				178
HandlerStub				179
HandlerStub				180
HandlerStub				181
HandlerStub				182
HandlerStub				183
HandlerStub				184
HandlerStub				185
HandlerStub				186
HandlerStub				187
HandlerStub				188
HandlerStub				189
HandlerStub				190
HandlerStub				191
HandlerStub				192
HandlerStub				193
HandlerStub				194
HandlerStub				195
HandlerStub				196
HandlerStub				197
HandlerStub				198
HandlerStub				199
HandlerStub				200
HandlerStub				201
HandlerStub				202
HandlerStub				203
HandlerStub				204
HandlerStub				205
HandlerStub				206
HandlerStub				207
HandlerStub				208
HandlerStub				209
HandlerStub				210
HandlerStub				211
HandlerStub				212
HandlerStub				213
HandlerStub				214
HandlerStub				215
HandlerStub				216
HandlerStub				217
HandlerStub				218
HandlerStub				219
HandlerStub				220
HandlerStub				221
HandlerStub				222
HandlerStub				223
HandlerStub				224
HandlerStub				225
HandlerStub				226
HandlerStub				227
HandlerStub				228
HandlerStub				229
HandlerStub				230
HandlerStub				231
HandlerStub				232
HandlerStub				233
HandlerStub				234
HandlerStub				235
HandlerStub				236
HandlerStub				237
HandlerStub				238
HandlerStub				239
HandlerStub				240
HandlerStub				241
HandlerStub				242
HandlerStub				243
HandlerStub				244
HandlerStub				245
HandlerStub				246
HandlerStub				247
HandlerStub				248
HandlerStub				249
HandlerStub				250
HandlerStub				251
HandlerStub				252
HandlerStub				253
HandlerStub				254
HandlerStub				255

section .data

GlobalHandlerStubTable:
	dq IntStub0
	dq IntStub1
	dq IntStub2
	dq IntStub3
	dq IntStub4
	dq IntStub5
	dq IntStub6
	dq IntStub7
	dq IntStub8
	dq IntStub9
	dq IntStub10
	dq IntStub11
	dq IntStub12
	dq IntStub13
	dq IntStub14
	dq IntStub15
	dq IntStub16
	dq IntStub17
	dq IntStub18
	dq IntStub19
	dq IntStub20
	dq IntStub21
	dq IntStub22
	dq IntStub23
	dq IntStub24
	dq IntStub25
	dq IntStub26
	dq IntStub27
	dq IntStub28
	dq IntStub29
	dq IntStub30
	dq IntStub31
	dq IntStub32
	dq IntStub33
	dq IntStub34
	dq IntStub35
	dq IntStub36
	dq IntStub37
	dq IntStub38
	dq IntStub39
	dq IntStub40
	dq IntStub41
	dq IntStub42
	dq IntStub43
	dq IntStub44
	dq IntStub45
	dq IntStub46
	dq IntStub47
	dq IntStub48
	dq IntStub49
	dq IntStub50
	dq IntStub51
	dq IntStub52
	dq IntStub53
	dq IntStub54
	dq IntStub55
	dq IntStub56
	dq IntStub57
	dq IntStub58
	dq IntStub59
	dq IntStub60
	dq IntStub61
	dq IntStub62
	dq IntStub63
	dq IntStub64
	dq IntStub65
	dq IntStub66
	dq IntStub67
	dq IntStub68
	dq IntStub69
	dq IntStub70
	dq IntStub71
	dq IntStub72
	dq IntStub73
	dq IntStub74
	dq IntStub75
	dq IntStub76
	dq IntStub77
	dq IntStub78
	dq IntStub79
	dq IntStub80
	dq IntStub81
	dq IntStub82
	dq IntStub83
	dq IntStub84
	dq IntStub85
	dq IntStub86
	dq IntStub87
	dq IntStub88
	dq IntStub89
	dq IntStub90
	dq IntStub91
	dq IntStub92
	dq IntStub93
	dq IntStub94
	dq IntStub95
	dq IntStub96
	dq IntStub97
	dq IntStub98
	dq IntStub99
	dq IntStub100
	dq IntStub101
	dq IntStub102
	dq IntStub103
	dq IntStub104
	dq IntStub105
	dq IntStub106
	dq IntStub107
	dq IntStub108
	dq IntStub109
	dq IntStub110
	dq IntStub111
	dq IntStub112
	dq IntStub113
	dq IntStub114
	dq IntStub115
	dq IntStub116
	dq IntStub117
	dq IntStub118
	dq IntStub119
	dq IntStub120
	dq IntStub121
	dq IntStub122
	dq IntStub123
	dq IntStub124
	dq IntStub125
	dq IntStub126
	dq IntStub127
	dq IntStub128
	dq IntStub129
	dq IntStub130
	dq IntStub131
	dq IntStub132
	dq IntStub133
	dq IntStub134
	dq IntStub135
	dq IntStub136
	dq IntStub137
	dq IntStub138
	dq IntStub139
	dq IntStub140
	dq IntStub141
	dq IntStub142
	dq IntStub143
	dq IntStub144
	dq IntStub145
	dq IntStub146
	dq IntStub147
	dq IntStub148
	dq IntStub149
	dq IntStub150
	dq IntStub151
	dq IntStub152
	dq IntStub153
	dq IntStub154
	dq IntStub155
	dq IntStub156
	dq IntStub157
	dq IntStub158
	dq IntStub159
	dq IntStub160
	dq IntStub161
	dq IntStub162
	dq IntStub163
	dq IntStub164
	dq IntStub165
	dq IntStub166
	dq IntStub167
	dq IntStub168
	dq IntStub169
	dq IntStub170
	dq IntStub171
	dq IntStub172
	dq IntStub173
	dq IntStub174
	dq IntStub175
	dq IntStub176
	dq IntStub177
	dq IntStub178
	dq IntStub179
	dq IntStub180
	dq IntStub181
	dq IntStub182
	dq IntStub183
	dq IntStub184
	dq IntStub185
	dq IntStub186
	dq IntStub187
	dq IntStub188
	dq IntStub189
	dq IntStub190
	dq IntStub191
	dq IntStub192
	dq IntStub193
	dq IntStub194
	dq IntStub195
	dq IntStub196
	dq IntStub197
	dq IntStub198
	dq IntStub199
	dq IntStub200
	dq IntStub201
	dq IntStub202
	dq IntStub203
	dq IntStub204
	dq IntStub205
	dq IntStub206
	dq IntStub207
	dq IntStub208
	dq IntStub209
	dq IntStub210
	dq IntStub211
	dq IntStub212
	dq IntStub213
	dq IntStub214
	dq IntStub215
	dq IntStub216
	dq IntStub217
	dq IntStub218
	dq IntStub219
	dq IntStub220
	dq IntStub221
	dq IntStub222
	dq IntStub223
	dq IntStub224
	dq IntStub225
	dq IntStub226
	dq IntStub227
	dq IntStub228
	dq IntStub229
	dq IntStub230
	dq IntStub231
	dq IntStub232
	dq IntStub233
	dq IntStub234
	dq IntStub235
	dq IntStub236
	dq IntStub237
	dq IntStub238
	dq IntStub239
	dq IntStub240
	dq IntStub241
	dq IntStub242
	dq IntStub243
	dq IntStub244
	dq IntStub245
	dq IntStub246
	dq IntStub247
	dq IntStub248
	dq IntStub249
	dq IntStub250
	dq IntStub251
	dq IntStub252
	dq IntStub253
	dq IntStub254
	dq IntStub255