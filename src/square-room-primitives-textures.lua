-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.5,0.5,1.0}, {0.5, 0.7, 0.5}, 67, 0.0, 0.5)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 85, 0.0, 0.5)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 55, 0.0, 0.5)
mat4 = gr.material({0.5,0.5,1.0}, {0.5, 0.4, 0.8}, 45, 0.0, 0.5)
mat5 = gr.material({0.5, 0.5, 1.0}, {0.3, 0.3, 0.5}, 85, 1.5, 1.0)
blue_wall = gr.material({0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, 1, 0.0, 0.0)
cyan_wall = gr.material({0.0, 1.0, 0.9}, {0.5, 0.5, 0.5}, 10, 0.0, 0.0)
purple_wall = gr.material({0.5, 0.5, 0.0}, {0.4, 0.4, 0.3}, 3, 0.0, 0.0)
white_wall = gr.material({0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}, 10, 0.0, 0.0)

scene_root = gr.node('root')
top_wall = gr.pgram('top_wall')
top_wall:set_material(white_wall)
top_wall:scale(10000, 10000, 10000);
top_wall:rotate('x', 90);
top_wall:translate(0, 701, -1200)

left_wall = gr.pgram('left_wall')
left_wall:set_material(white_wall)
left_wall:scale(10000,10000, 10000);
left_wall:rotate('y', 90);
left_wall:translate(-600, 0, 0);

right_wall = gr.pgram('right_wall')
right_wall:set_material(white_wall)
right_wall:scale(10000,10000, 10000);
right_wall:rotate('y', -90);
right_wall:translate(600, 0, 0);

back_wall = gr.pgram('back_wall')
back_wall:set_material(white_wall)
back_wall:scale(10000, 10000, 10000);
back_wall:translate(0, 0, -2000);


light_area1 = gr.pgram('larea1')
light_area1:set_material(mat1);
light_area1:scale(200, 200, 200)
light_area1:translate(0, 700, -1200)
light_area1:rotate('x', 90)

light_area2 = gr.pgram('larea2')
light_area2:set_material(mat1)
light_area2:scale(200, 200, 200)
light_area2:translate(597, 0, -1200)
light_area2:rotate('y', 90)

bright_light = gr.light({0, 698, -1200}, {1.0, 1.0, 1.0}, {1, 0, 0}, {400,0,0}, {0,0,400}, 90)
orange_light = gr.light({595, 0, -1200}, {1.0, 1.0, 1.0}, {1, 0, 0}, {0, 400, 0}, {0, 0, 400}, 90)



--top_wall:set_texture("checker.png")
--left_wall:set_texture("checker.png")
--back_wall:set_texture("checker.png")
--right_wall:set_texture("checker.png")
--top_wall:set_bump("normal_4.png")
--left_wall:set_bump("normal_4.png")
--back_wall:set_bump("normal_4.png")
--right_wall:set_bump("normal_4.png")

scene_root:add_child(light_area1)
scene_root:add_child(light_area2)
scene_root:add_child(top_wall)
scene_root:add_child(left_wall)
scene_root:add_child(right_wall)
scene_root:add_child(back_wall);

top_wall:set_texture_scale(20, 20)
left_wall:set_texture_scale(20, 20)
back_wall:set_texture_scale(20, 20)
right_wall:set_texture_scale(20, 20)

s1 = gr.nh_sphere('s1', {0, -400, -800}, 100)
s2 = gr.nh_sphere('s2', {-400, -400, -1200}, 100)
s3 = gr.nh_sphere('s3', {400, -400, -1200}, 100)
s4 = gr.nh_sphere('s4', {0, -400, -1600}, 100)
s5 = gr.nh_sphere('s5', {-400, -400, -800}, 100)
s6 = gr.nh_sphere('s6', {400, -400, -800}, 100)
s7 = gr.nh_sphere('s7', {400, -400, -1600}, 100)
s8 = gr.nh_sphere('s8', {-400, -400, -1600}, 100)
scene_root:add_child(s1)
scene_root:add_child(s2)
scene_root:add_child(s3)
scene_root:add_child(s4)
scene_root:add_child(s5)
scene_root:add_child(s6)
scene_root:add_child(s7)
scene_root:add_child(s8)


s1:set_material(mat1)
s2:set_material(mat1)
s3:set_material(mat1)
s4:set_material(mat1)
s5:set_material(mat1)
s6:set_material(mat1)
s7:set_material(mat1)
s8:set_material(mat1)

c1 = gr.cone('c1')
--c1:set_bump("rock_bump.png")

c1:scale(200.0, 200.0, 600.0)

c2 = gr.cylinder('c2', {0, 0, 0}, {0, 0, 1}, 1)
c2:scale(100, 100, 314)
--c2:set_bump("rock_bump.png")

c2:rotate('x', -70)
c2:translate(-300, -100, -1700);

c1:rotate('x', 70)
c1:translate(0, 0, -1500);

c1:set_material(mat1)
c2:set_material(mat3)

s9 = gr.nh_sphere('s9', {0, 300, -400}, 200)
s9:set_material(mat1)
--s9:set_bump("rock_bump.png")
s9:set_texture("checker.png")

scene_root:add_child(c2)
scene_root:add_child(c1)
scene_root:add_child(s9)

gr.associate(bright_light, light_area1)
gr.associate(orange_light, light_area2)

b1 = gr.nh_box('b1', {-500, -1500, -1700}, 1000)

b1:set_texture("checker.png")
c1:set_texture("checker.png")
c2:set_texture("checker.png")
--b1:set_bump("normal_4.png")

scene_root:add_child(b1)
b1:set_material(mat3)

gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {bright_light, orange_light}, 75.0, 1700.0)
