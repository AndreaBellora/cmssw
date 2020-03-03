#!/bin/bash
voms-proxy-init --voms cms --rfc --valid 120:00
echo "Copying certificate in the current directory..."
cp "/tmp/x509up_u$UID" .