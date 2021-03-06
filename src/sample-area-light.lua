-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.5,0.5,1.0}, {0.5, 0.7, 0.5}, 67, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 85, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 55, 0.0, 0.2)
mat4 = gr.material({0.5,0.5,1.0}, {0.5, 0.4, 0.8}, 45, 0.0, 0.2)
mat5 = gr.material({0.5, 0.5, 1.0}, {0.0, 0.0, 0.0}, 85, 0.0, 0.0)
mat6 = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 1, 0.0, 0.0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, -400, -800}, 100)
s2 = gr.nh_sphere('s2', {-400, -400, -1200}, 100)
s3 = gr.nh_sphere('s3', {400, -400, -1200}, 100)
s4 = gr.nh_sphere('s4', {0, -400, -1600}, 100)
s5 = gr.nh_sphere('s5', {-400, -400, -800}, 100)
s6 = gr.nh_sphere('s6', {400, -400, -800}, 100)
s7 = gr.nh_sphere('s7', {400, -400, -1600}, 100)
s8 = gr.nh_sphere('s8', {-400, -400, -1600}, 100)

light_area1 = gr.pgram('larea1')
light_area1:set_material(mat6);
light_area1:scale(200, 200, 1)
light_area1:translate(0, 700, -1200)
light_area1:rotate('x', 90)

bright_light = gr.light({0, 695, -1200}, {1.0, 1.0, 1.0}, {1, 0, 0}, {400,0,0}, {0,0,400}, 30)

scene_root:add_child(s1)
scene_root:add_child(s2)
scene_root:add_child(s3)
scene_root:add_child(s4)
scene_root:add_child(s5)
scene_root:add_child(s6)
scene_root:add_child(s7)
scene_root:add_child(s8)
scene_root:add_child(light_area1)

gr.associate(bright_light, light_area1)

s1:set_material(mat1)
s2:set_material(mat1)
s3:set_material(mat1)
s4:set_material(mat1)
s5:set_material(mat1)
s6:set_material(mat1)
s7:set_material(mat1)
s8:set_material(mat1)

b1 = gr.nh_box('b1', {-500, -1500, -1700}, 1000)
for j=0,2 do
	for i=0,6 do
		b2 = gr.nh_box('b2', {300, -300 + 100 * i, (-800 - j*400) - 100}, 200); 
		scene_root:add_child(b2)
		b2:set_material(mat2)
		 
	end
end

scene_root:add_child(b1)
b1:set_material(mat3)

white_light = gr.light({2000, 200, -1200}, {0.8, 0.8, 0.3}, {1, 0, 0}, {0,400,0}, {0,0,400}, 0)
orange_light = gr.light({0, 300, 1200}, {0.7, 0.5, 0.3}, {1, 0, 0}, {400,0,0}, {0,400,0}, 0)

gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {bright_light}, 75.0, 1200.0)
