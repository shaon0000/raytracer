-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.5,0.5,1.0}, {0.5, 0.7, 0.5}, 67, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 85, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 55, 0.0, 0.2)
mat4 = gr.material({0.5,0.5,1.0}, {0.5, 0.4, 0.8}, 45, 0.0, 0.2)
mat5 = gr.material({0.5, 0.5, 1.0}, {0.0, 0.0, 0.0}, 85, 0.0, 0.2)

mat_white_light = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 1, 0.0, 0.0)

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, -400, -1200}, 100)
s2 = gr.cone('s2')
s3 = gr.nh_sphere('s3', {400, -400, -1200}, 100)
s4 = gr.cylinder('s4', {0, 0, 0}, {0, 0, 1}, 1)
s5 = gr.nh_sphere('s5', {0, 0, -3000}, 100)
s6 = gr.nh_sphere('s6', {400, -400, -800}, 1)
s7 = gr.nh_sphere('s7', {400, -400, -1600}, 100)
s8 = gr.nh_sphere('s8', {-400, -400, -1600}, 100)

sq_back = gr.pgram('back')
sq_back:set_material(mat_white_light);
sq_back:scale(2000, 2000, 1)
sq_back:translate(0, 0, -1600)

sq_left = gr.pgram('left')
sq_left:set_material(mat_white_light);
sq_left:scale(2000, 2000, 1);
sq_left:rotate('y', 90)
sq_left:translate(-800, 0, -1100);

sq_bottom = gr.pgram('bottom')


scene_root:add_child(sq_back)
scene_root:add_child(sq_left)

scene_root:add_child(s1)
scene_root:add_child(s2)
scene_root:add_child(s3)
scene_root:add_child(s4)
scene_root:add_child(s5)
scene_root:add_child(s6)
scene_root:add_child(s7)
scene_root:add_child(s8)
--

s4:scale(100, 100, 314)

s4:rotate('z', 90)
s4:rotate('x', -90)
s4:translate(0, -100, 0);

--s4:set_texture("rock_bump.png")
--s4:set_bump("rock_bump.png")

s1:set_material(mat1)
s2:set_material(mat4)
s3:set_material(mat1)
s4:set_material(mat4)
s5:set_material(mat1)
s6:set_material(mat4)
s7:set_material(mat1)
s8:set_material(mat4)


s2:scale(200.0, 200.0, 600.0)

s2:rotate('x', -90.0)
s2:translate(0, -300, -1600);

s9:translate(0, -300 + 600, -1600);

s9:set_texture("rock_bump.png")
s9:set_bump("rock_bump.png")
--s6:set_texture("world.png")
--s6:set_bump("world_bump.png")



s2:set_bump("rock_bump.png")

s6:scale(300.0, 300.0, 300.0);

b1 = gr.nh_box('b1', {0, 0, 0}, 1000)
b1:translate(-500, -1500, -1700)
for j=0,2 do
	for i=0,6 do
		b2 = gr.nh_box('b2', {(j + 1) * (200 * i) + 100, -50 + 100 * i, -3000}, 200); 
		scene_root:add_child(b2)
		b2:set_material(mat2)
		 
	end
end

--scene_root:add_child(b1)
b1:set_material(mat3)
b1:set_texture("rock_bump.png")
b1:set_bump("rock_bump.png")
b1:rotate('y', 45)

-- A small stellated dodecahedron.

-- require('smstdodeca')

-- steldodec:set_material(mat3)
-- scene_root:add_child(steldodec)

white_light = gr.light({2000, 200, -1200}, {0.8, 0.8, 0.3}, {1, 0, 0}, {0,400,0}, {0,0,400}, 90)
orange_light = gr.light({0, 300, 1200}, {1.0, 1.0, 1.0}, {1, 0, 0}, {400,0,0}, {0,400,0}, 90)

gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light}, 75.0, 1200.0)
