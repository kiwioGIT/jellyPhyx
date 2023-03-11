# import the pygame module, so you can use it
import pygame
import math
# define a main function

points = []
springs = []

default_stifness = 0.3
default_damping = 0.1



def distance(x1,y1,x2,y2):
    return math.sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2))


def closest_point(x,y):
    min_dist = 999999
    closest_point = -1
    for p in range(len(points)):
        dist = distance(points[p][0],points[p][1],x,y)
        if dist < min_dist:
            min_dist = dist
            closest_point = p
    return closest_point


def save():
    print("Saving to file")
    f= open("body.phx","w+")
    f.write("m 1 2 2" + "\n")
    for point in points:
        f.write("p " + str(point[0]) + " " + str(point[1]) + "\n")
    for spring in springs:
        f.write("s " + str(spring[0]) + " " + str(spring[1]) + " " + str(distance(points[spring[0]][0],points[spring[0]][1],points[spring[1]][0],points[spring[1]][1])) + " " + str(default_stifness) + " " + str(default_damping) + "\n")
    for os in range(len(points) - 1):
        f.write("o " + str(os) + " " + str(os + 1) + "\n")
    f.write("o " + str(len(points) - 1) + " " + str(0))



def main():
    pygame.init()
    pygame.display.set_caption("PhysicsObjectCreator")
    screen = pygame.display.set_mode((800,600))

    running = True
    
    saved_point = -1

    while running:
        

        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_s:
                    save()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if event.button == 1:
                    points.append([pygame.mouse.get_pos()[0],pygame.mouse.get_pos()[1]])
                if event.button == 3:
                    print(saved_point)
                    if saved_point == -1:
                        saved_point = closest_point(pygame.mouse.get_pos()[0],pygame.mouse.get_pos()[1])
                    else:
                        springs.append([saved_point,closest_point(pygame.mouse.get_pos()[0],pygame.mouse.get_pos()[1])])
                        saved_point = -1
            if event.type == pygame.QUIT:
                running = False
        screen.fill((0,0,0))
        for s in range(len(points)):
            pygame.draw.line(screen, (0,0,255), points[s - 1], points[s],5)
        for spring in springs:
            pygame.draw.line(screen, (0,255,0,128), points[spring[0]], points[spring[1]],5)
        for p in points:
            pygame.draw.circle(screen, (255,0,0,128), (p[0], p[1]), 5, 5)
        pygame.display.update()
    pygame.quit()




if __name__=="__main__":
    main()