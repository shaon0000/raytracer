-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.5,0.5,1.0}, {0.5, 0.7, 0.5}, 67, 0.0, 0.2)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 85, 0.0, 0.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 55, 0.0, 0.2)
mat4 = gr.material({0.5,0.5,1.0}, {0.5, 0.4, 0.8}, 45, 0.0, 0.2)
mat5 = gr.material({0.5, 0.5, 1.0}, {0.0, 0.0, 0.0}, 85, 0.0, 0.0)
blue_wall = gr.material({0.0, 0.0, 1.0}, {1.0, 1.0, 1.0}, 1, 0.0, 0.0)
cyan_wall = gr.material({0.0, 1.0, 0.9}, {0.5, 0.5, 0.5}, 10, 0.0, 0.0)
purple_wall = gr.material({0.5, 0.5, 0.0}, {0.4, 0.4, 0.3}, 3, 0.0, 0.0)


scene_root = gr.node('root')
top_wall = gr.pgram('top_wall')
top_wall:set_material(blue_wall)
top_wall:scale(10000, 10000, 10000);
top_wall:rotate('x', 90);
top_wall:translate(0, 701, -1200)

left_wall = gr.pgram('left_wall')
left_wall:set_material(cyan_wall)
left_wall:scale(10000,10000, 10000);
left_wall:rotate('y', 90);
left_wall:translate(-600, 0, 0);

right_wall = gr.pgram('right_wall')
right_wall:set_material(cyan_wall)
right_wall:scale(10000,10000, 10000);
right_wall:rotate('y', 90);
right_wall:translate(600, 0, 0);

back_wall = gr.pgram('back_wall')
back_wall:set_material(purple_wall)
back_wall:scale(10000, 10000, 10000);
back_wall:translate(0, 0, -2000);

light_area1 = gr.pgram('larea1')
light_area1:set_material(mat1);
light_area1:scale(200, 200, 1)
light_area1:translate(0, 700, -1200)
light_area1:rotate('x', 90)

light_area2 = gr.pgram('larea2')
light_area2:set_material(mat1)
light_area2:scale(200, 200, 200)
light_area2:translate(597, 0, -1200)
light_area2:rotate('y', 90)


bright_light = gr.light({0, 698, -1200}, {0.5, 0.0, 0.0}, {1, 0, 0}, {400,0,0}, {0,0,400}, 60)
orange_light = gr.light({595, 0, -1200}, {0.0, 0.5, 0.0}, {1, 0, 0}, {0, 400, 0}, {0, 0, 400}, 60)

top_wall:set_texture("checker.png")
left_wall:set_texture("checker.png")
back_wall:set_texture("checker.png")
right_wall:set_texture("checker.png")
top_wall:set_bump("normal_4.png")
left_wall:set_bump("normal_4.png")
back_wall:set_bump("normal_4.png")
right_wall:set_bump("normal_4.png")

scene_root:add_child(light_area1)
--scene_root:add_child(light_area2)
scene_root:add_child(top_wall)
scene_root:add_child(left_wall)
scene_root:add_child(right_wall)
scene_root:add_child(back_wall);

top_wall:set_texture_scale(20, 20)
left_wall:set_texture_scale(20, 20)
back_wall:set_texture_scale(20, 20)
right_wall:set_texture_scale(20, 20)


gr.associate(bright_light, light_area1)
--gr.associate(orange_light, light_area2)

b1 = gr.nh_box('b1', {-500, -1500, -1700}, 1000)

b1:set_texture("checker.png")
b1:set_bump("normal_4.png")

--scene_root:add_child(b1)
b1:set_material(mat3)

gr.render(scene_root, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {bright_light}, 75.0, 1200.0)
