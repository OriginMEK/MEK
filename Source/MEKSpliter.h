# p r a g m a   o n c e  
 / *  
 � � � � � �  
 * /  
 # i f n d e f   M E K S P L I T E R _ H _  
 # d e f i n e   M E K S P L I T E R _ H _  
 # i n c l u d e   " M E K I n c l u d e D e f . h "  
 c l a s s   M E K S p l i t e r  
 {  
 p u b l i c :  
 	 M E K S p l i t e r ( ) ;  
 	 ~ M E K S p l i t e r ( ) ;  
  
 	 b o o l   A n a l y z e F i l e ( c h a r *   f i l e N a m e , M E K P a r a m *   d a t a ) ;  
 	 s t a t i c   v o i d   S t a r t ( v o i d   * p a r a m )  
 	 {  
 	 	 M E K S p l i t e r *   s p l i t e r   =   ( M E K S p l i t e r * ) p a r a m ;  
 	 	 s p l i t e r - > S p l i t e r T h r e a d ( ) ;  
 	 }  
  
 p r i v a t e :  
 	 M E K P a r a m *   m D a t a ;  
 	 i n t   O p e n C o d e c C o n t e x t ( i n t   * s t r e a m I n d e x ,   A V C o d e c C o n t e x t   * * d e c _ c t x ,   A V F o r m a t C o n t e x t   * f m t _ c t x ,   e n u m   A V M e d i a T y p e   t y p e ) ;  
 	 v o i d   S p l i t e r T h r e a d ( ) ;  
 } ;  
 # e n d i f   / / M E K S P L I T E R _ H _ 