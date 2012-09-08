#!/bin/sh

rm -rf bun.csv
touch bun.csv

./main 128 128 ./scenes/bunny/reconstruction/bun_zipper_res4.ply bun_zipper_res4_128.png | tee -a bun.csv
./main 256 256 ./scenes/bunny/reconstruction/bun_zipper_res4.ply bun_zipper_res4_256.png | tee -a bun.csv
./main 512 512 ./scenes/bunny/reconstruction/bun_zipper_res4.ply bun_zipper_res4_512.png | tee -a bun.csv
./main 1024 1024 ./scenes/bunny/reconstruction/bun_zipper_res4.ply bun_zipper_res4_1k.png | tee -a bun.csv

./main 128 128 ./scenes/bunny/reconstruction/bun_zipper_res3.ply bun_zipper_res3_128.png | tee -a bun.csv
./main 256 256 ./scenes/bunny/reconstruction/bun_zipper_res3.ply bun_zipper_res3_256.png | tee -a bun.csv
./main 512 512 ./scenes/bunny/reconstruction/bun_zipper_res3.ply bun_zipper_res3_512.png | tee -a bun.csv
./main 1024 1024 ./scenes/bunny/reconstruction/bun_zipper_res3.ply bun_zipper_res3_1k.png | tee -a bun.csv

./main 128 128 ./scenes/bunny/reconstruction/bun_zipper_res2.ply bun_zipper_res2_128.png | tee -a bun.csv
./main 256 256 ./scenes/bunny/reconstruction/bun_zipper_res2.ply bun_zipper_res2_256.png | tee -a bun.csv
./main 512 512 ./scenes/bunny/reconstruction/bun_zipper_res2.ply bun_zipper_res2_512.png | tee -a bun.csv
./main 1024 1024 ./scenes/bunny/reconstruction/bun_zipper_res2.ply bun_zipper_res2_1k.png | tee -a bun.csv

./main 128 128 ./scenes/bunny/reconstruction/bun_zipper.ply bun_zipper_res1_128.png | tee -a bun.csv
./main 256 256 ./scenes/bunny/reconstruction/bun_zipper.ply bun_zipper_res1_256.png | tee -a bun.csv
./main 512 512 ./scenes/bunny/reconstruction/bun_zipper.ply bun_zipper_res1_512.png | tee -a bun.csv
./main 1024 1024 ./scenes/bunny/reconstruction/bun_zipper.ply bun_zipper_res1_1k.png | tee -a bun.csv


